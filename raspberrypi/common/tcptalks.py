#!/usr/bin/python3
#-*- coding: utf-8 -*-

import socket
import pickle
from time      import time
from queue     import Queue, Empty
from threading import Thread, RLock, Event, current_thread

MASTER_BYTE = b'R'
SLAVE_BYTE  = b'A'

AUTHENTIFICATION_OPCODE = 0xAA
DISCONNECT_OPCODE       = 0xFF


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


def _loads(rawbytes):
	a, b = 0, len(rawbytes)
	while (b - a > 1):
		i = (a + b) // 2
		try:
			output = pickle.loads(rawbytes[:i])
		except (EOFError, pickle.UnpicklingError, AttributeError):
			a = i
		else:
			b = i
	return pickle.loads(rawbytes[:b]), rawbytes[b:]


class TCPTalks:

	def __init__(self, ip=None, port=25565, password=None):
		Thread.__init__(self)

		# Instructions
		self.instructions = dict()
		self.bind(DISCONNECT_OPCODE, self.disconnect)

		# Socket things
		self.ip   = ip
		self.port = port
		self.is_connected = False

		# Password
		self.password = password

		# Thread-safe inputs polling
		self.queues_dict = dict()
		self.queues_lock = RLock()

	def __enter__(self):
		self.connect()
		return self

	def __exit__(self, exc_type, exc_value, traceback):
		self.disconnect()

	def connect(self, timeout=2):
		if self.is_connected:
			raise RuntimeError('already connected')
			
		# The remote controller is not able to execute custom instructions
		# as long as the following flag remains unset
		self.is_authentificated = False
		
		# This will let the Raspberry Pi try to reconnect itself after a
		# bad authentification
		while not self.is_connected:
			# Create a socket instance depending of what was given during the
			# instanciation
			if self.ip is None: # Raspberry Pi
				self.socket = _serversocket(self.port, timeout)
			else: # Remote controller (Windows or Linux)
				self.socket = _clientsocket(self.ip, self.port, timeout)
			self.socket.settimeout(1)
			
			# Create a listening thread that will wait for inputs
			self.listener = TCPListener(self)
			self.listener.start()

			# This flag is a read-only attribute indicating whether or not the
			# current instance is connected to the other
			self.is_connected = True

			# Password authentification
			if self.ip is None: # Raspberry Pi
				if not self.wait_for_authentification(1):
					self.disconnect()
			else: # Remote controller
				if not self.authentificate():
					self.disconnect()
					raise RuntimeError('authentification failed')

	def authentificate(self):
		self.sendback(AUTHENTIFICATION_OPCODE, self.password)
		self.is_authentificated = self.poll(AUTHENTIFICATION_OPCODE, None)
		return self.is_authentificated

	def wait_for_authentification(self, timeout):
		password = self.poll(AUTHENTIFICATION_OPCODE, timeout)
		self.is_authentificated = self.password in (None, password)
		self.sendback(AUTHENTIFICATION_OPCODE, self.is_authentificated)
		return self.is_authentificated

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
			raise RuntimeError('not connected')
		
		sentbytes = 0
		while(sentbytes < len(rawbytes)):
			sentbytes += self.socket.send(rawbytes[sentbytes:])
		return sentbytes

	def send(self, opcode, *args, **kwargs):
		content = (opcode, args, kwargs)
		prefix  = (MASTER_BYTE,)
		return self.rawsend(pickle.dumps(prefix + content))

	def sendback(self, opcode, *args):
		content = (opcode, args)
		prefix  = (SLAVE_BYTE,)
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
		if (role == MASTER_BYTE):
			kwargs = message[3]
			self.execinstruction(opcode, *args, **kwargs)
		elif (role == SLAVE_BYTE):
			queue = self.get_queue(opcode)
			queue.put(args)

	def execinstruction(self, opcode, *args, **kwargs):
		try:
			# Make sure that the authentification was well performed
			if not self.is_authentificated:
				raise RuntimeError('you are not authentificated')

			# Get the function or method associated with the received opcode
			try:
				instruction = self.instructions[opcode]
			except KeyError:
				raise KeyError('opcode {} is not bound to any instruction'.format(opcode)) from None
			
			# Execute the instruction and send back its output
			return self.sendback(opcode, instruction(*args, **kwargs))
		except Exception as e:
			return self.sendback(opcode, e)

	def poll(self, opcode, timeout=0):	
		queue = self.get_queue(opcode)
		block = (timeout is None or timeout > 0)
		try:
			output = queue.get(block, timeout)
		except Empty:
			return None
		if len(output) > 1:
			return output # Return as a tuple
		else:
			return output[0] # Return as a single variable
	
	def flush(self, opcode):
		while self.poll(opcode) is not None:
			pass
	
	def execute(self, opcode, *args, timeout=1, **kwargs):
		self.flush(opcode)
		self.send(opcode, *args, **kwargs)
		output = self.poll(opcode, timeout=timeout)
		if isinstance(output, Exception):
			raise output
		else:
			return output

	def sleep_until_disconnected(self):
		if self.listener is not current_thread():
			self.listener.join()
		else:
			raise RuntimeError('cannot sleep the listening thread')


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
			except ConnectionResetError:
				inc = None
			except socket.timeout:
				continue
			
			# Disconnect if the other is no longer connected
			if not inc:
				self.parent.disconnect()
				break
			
			# Try to decode the message using the pickle protocol
			buffer += inc
			try:
				message, buffer = _loads(buffer)
			except (EOFError, pickle.UnpicklingError, AttributeError):
				continue
			self.parent.process(message)

