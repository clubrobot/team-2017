#!/usr/bin/python3
#-*- coding: utf-8 -*-

import socket
import pickle
from time      import time
from queue     import Queue, RLock, Empty
from threading import Thread, Event, current_thread

MASTER_BYTE = b'R'
SLAVE_BYTE  = b'A'

DISCONNECT_OPCODE = 0xFF


def _serversocket(self, port, timeout):
	# Create a server
	serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	serversocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	serversocket.bind(('', port))
	serversocket.listen(1)

	# Wait for the other to connect
	serversocket.settimeout(timeout)
	try:
		clientsocket = serversocket.accept()[0]
		serversocket.close() # The server is no longer needed
		return clientsocket
	except socket.timeout:
		raise TimeoutError('no connection request')


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
	raise TimeoutError('no server found')


class TCPTalks:

	def __init__(self, ip = None, port = 25565):
		Thread.__init__(self)

		# Socket things
		self.ip   = ip
		self.port = port
		self.is_connected = False

		# Instructions
		self.instructions = dict()

		# Threading things
		self.queues_dict = dict()
		self.queues_lock = RLock()
		self.stop_event  = Event()

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
			self.socket.settimeout(0)
			self.is_connected = True
			
			# Create a listening thread that will wait for inputs and 
			self.listener = TCPListener(self)
			self.listener.start()
		else:
			if self.ip is not None:
				raise RuntimeError('{} is already connected'.format(self.ip))
			else:
				raise RuntimeError('client is already connected')

	def disconnect(self):
		if self.is_connected:
			# Send a disconnect notification to the other
			self.send(DISCONNECT_OPCODE)

			# Stop the listening thread
			self.stop_event.set()
			if self.listener is not current_thread():
				self.listener.join()

			# Close the socket
			self.socket.close()
			self.is_connected = False

	def bind(self, opcode, instruction):
		if not opcode in self.instructions:
			self.instructions[opcode] = instruction
		else:
			raise KeyError('opcode {} is already bound to another instruction'.format(opcode))

	def rawsend(self, rawbytes):
		if not self.is_connected():
			if self.ip is not None:
				raise RuntimeError('{} is not connected'.format(self.ip))
			else:
				raise RuntimeError('client is not connected')
		
		sentbytes = 0
		while(sentbytes < len(rawbytes)):
			sentbytes += self.server.send(rawbytes[sentbytes:])
		return sentbytes

	def send(self, opcode, *args):
		content = [opcode] + list(args)
		prefix  = [MASTER_BYTE]
		return self.rawsend(pickle.dumps(prefix + content))

	def sendback(self, opcode, *args):
		content = [opcode] + list(args)
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

	def process(self, inc):
		role   = inc[0]
		opcode = inc[1]
		if (role == MASTER_BYTE):
			instruction = self.instructions[opcode]
			instruction(*inc[2:])
		elif (direction == SLAVE_BYTE):
			queue = self.get_queue(opcode)
			queue.put(inc[2:])

	def poll(self, opcode, timeout = 0):	
		queue = self.get_queue(opcode)
		block = (timeout is None or timeout > 0)
		try:
			return tuple(queue.get(block, timeout))
		except Empty:
			return None
	
	def flush(self, opcode):
		while self.poll(opcode) is not None:
			pass


class TCPListener(Thread):

	def __init__(self, parent):
		self.talks  = parent
		self.daemon = True

	def run(self):
		while not self.parent.stop_event.is_set():
			try:
				inc = parent.socket.recv(1024)
			except BlockingIOError:
				continue
			self.parent.process(pickle.loads(inc))

