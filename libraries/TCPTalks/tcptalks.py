#!/usr/bin/python3
#-*- coding: utf-8 -*-

import socket
import pickle
from time      import time
from queue     import Queue, Empty
from threading import Thread, RLock, Event, current_thread

MASTER_BYTE = b'R'
SLAVE_BYTE  = b'A'

DISCONNECT_OPCODE = 0xFF


def _serversocket(port, timeout):
	# Create a server
	serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	serversocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	serversocket.bind(('', port))
	serversocket.listen(1)

	# Wait for the other to connect
	serversocket.settimeout(timeout)
	try:
		clientsocket = serversocket.accept()[0]
		return clientsocket
	except socket.timeout:
		raise TimeoutError('no connection request') from None
	finally:
		serversocket.close() # The server is no longer needed


def _clientsocket(ip, port, timeout):
	# Create a client
	clientsocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

	# Connect to the other
	startingtime = time()
	while timeout is None or time() - startingtime < timeout:
		try:
			clientsocket.connect((ip, port))
			return clientsocket
		except ConnectionRefusedError:
			continue
	raise TimeoutError('no server found') from None


class TCPTalks:

	def __init__(self, ip = None, port = 25565):
		Thread.__init__(self)

		# Socket things
		self.ip   = ip
		self.port = port
		self.is_connected = False

		# Instructions
		self.instructions = dict()
		self.bind(DISCONNECT_OPCODE, self.disconnect)

		# Thread-safe inputs polling
		self.queues_dict = dict()
		self.queues_lock = RLock()

	def __enter__(self):
		self.connect()
		return self

	def __exit__(self, exc_type, exc_value, traceback):
		self.disconnect()

	def connect(self, timeout = 2):
		if not self.is_connected:
			# Create a socket instance depending of what was given during the initialization
			if self.ip is None: # Raspberry Pi
				self.socket = _serversocket(self.port, timeout)
			else: # Remote controller (Windows or Linux)
				self.socket = _clientsocket(self.ip, self.port, timeout)
			self.socket.settimeout(1)
			
			# Create a listening thread that will wait for inputs
			self.listener = TCPListener(self)
			self.listener.start()

			self.is_connected = True
		else:
			if self.ip is not None:
				raise RuntimeError('{} is already connected'.format(self.ip))
			else:
				raise RuntimeError('client is already connected')

	def disconnect(self):
		if self.is_connected:
			self.is_connected = False

			# Send a disconnect notification to the other
			try:
				self.send(DISCONNECT_OPCODE)
			except RuntimeError:
				pass

			# Stop the listening thread
			self.listener.stop.set()
			if self.listener is not current_thread():
				self.listener.join()

			# Close the socket
			self.socket.close()

	def bind(self, opcode, instruction):
		if not opcode in self.instructions:
			self.instructions[opcode] = instruction
		else:
			raise KeyError('opcode {} is already bound to another instruction'.format(opcode))

	def rawsend(self, rawbytes):
		if not self.is_connected:
			if self.ip is not None:
				raise RuntimeError('{} is not connected'.format(self.ip))
			else:
				raise RuntimeError('client is not connected')
		
		sentbytes = 0
		while(sentbytes < len(rawbytes)):
			sentbytes += self.socket.send(rawbytes[sentbytes:])
		return sentbytes

	def send(self, opcode, *args, **kwargs):
		content = [opcode] + [args, kwargs]
		prefix  = [MASTER_BYTE]
		return self.rawsend(pickle.dumps(prefix + content))

	def sendback(self, opcode, *args, **kwargs):
		content = [opcode] + [args, kwargs]
		prefix  = [SLAVE_BYTE]
		return self.rawsend(pickle.dumps(prefix + content))

	def get_queue(self, opcode):
		self.queues_lock.acquire()
		try:
			queue = self.queues_dict[opcode]
		except KeyError:
			queue = self.queues_dict[opcode] = Queue()
		finally:
			self.queues_lock.release()
		return queue

	def process(self, message):
		role   = message[0]
		opcode = message[1]
		args   = message[2]
		kwargs = message[3]
		if (role == MASTER_BYTE):
			instruction = self.instructions[opcode]
			try:
				instruction(*args, **kwargs)
			except Exception as e:
				self.sendback(opcode, e)
		elif (role == SLAVE_BYTE):
			queue = self.get_queue(opcode)
			queue.put(args)

	def poll(self, opcode, timeout = 0):	
		queue = self.get_queue(opcode)
		block = (timeout is None or timeout > 0)
		try:
			output = queue.get(block, timeout)
			return tuple(output) if len(output) > 1 else output[0]
		except Empty:
			return None
	
	def flush(self, opcode):
		while self.poll(opcode) is not None:
			pass


class TCPListener(Thread):

	def __init__(self, parent):
		Thread.__init__(self)
		self.parent = parent
		self.stop   = Event()
		self.daemon = True

	def run(self):
		buffer = bytes()
		while not self.stop.is_set():
			# Wait until new bytes arrive
			try:
				inc = self.parent.socket.recv(256)
			except socket.timeout:
				continue
			
			# Disconnect if the other is no longer connected
			if len(inc) == 0:
				self.parent.disconnect()
			
			# Try to decode the message using the pickle protocol
			for b in inc:
				buffer += bytes([b])
				try:
					message = pickle.loads(buffer)
				except (EOFError, pickle.UnpicklingError, AttributeError):
					continue
				self.parent.process(message)
				buffer = bytes()

