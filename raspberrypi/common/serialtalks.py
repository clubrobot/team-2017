#!/usr/bin/python3
#-*- coding: utf-8 -*-

import sys
import serial
import time
from queue		import Queue, Empty
from threading	import Thread, RLock, Event, current_thread

from common.serialutils import Deserializer, IntegerType, FloatType, StringType

BAUDRATE = 115200

MASTER_BYTE = b'R'
SLAVE_BYTE  = b'A'

CONNECT_OPCODE = 0x00
GETUUID_OPCODE = 0x01
SETUUID_OPCODE = 0x02
STDOUT_OPCODE  = 0xFF
STDERR_OPCODE  = 0xFE

BYTEORDER = 'little'
ENCODING  = 'utf-8'

CHAR   = IntegerType(1, BYTEORDER, True)
UCHAR  = IntegerType(1, BYTEORDER, False)
SHORT  = IntegerType(2, BYTEORDER, True)
USHORT = IntegerType(2, BYTEORDER, False)
LONG   = IntegerType(4, BYTEORDER, True)
ULONG  = IntegerType(4, BYTEORDER, False)

FLOAT  = FloatType('f')

STRING = StringType(ENCODING)

BYTE   = UCHAR
INT    = SHORT
UINT   = USHORT
DOUBLE = FLOAT


# Exceptions

class AlreadyConnectedError(ConnectionError): pass
class ConnectionFailedError(ConnectionError): pass
class NotConnectedError    (ConnectionError): pass
class MuteError(TimeoutError): pass


# Main class

class SerialTalks:

	def __init__(self, port):
		Thread.__init__(self)

		# Serial things
		self.port = port
		self.is_connected = False

		# Threading things
		self.queues_dict = dict()
		self.queues_lock = RLock()

	def __enter__(self):
		self.connect()
		return self

	def __exit__(self, exc_type, exc_value, traceback):
		self.disconnect()

	def connect(self, timeout=2):
		if not self.is_connected:
			try:
				self.stream = serial.Serial(self.port, baudrate=BAUDRATE, bytesize=serial.EIGHTBITS, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE)
			except serial.serialutil.SerialException as e:
				raise ConnectionFailedError(str(e))

			self.stream.timeout	= 1
		
			# Create a listening thread that will wait for inputs
			self.listener = SerialListener(self)
			self.listener.start()

			self.is_connected = True

			# Wait until the Arduino is operational
			startingtime = time.time()
			while timeout is None or time.time() - startingtime < timeout:
				if self.execute(CONNECT_OPCODE, timeout=0.1) is not None:
					break
			else:
				self.disconnect()
				raise MuteError('\'{}\' is mute. It may not be an Arduino or it\'s sketch may not be correctly loaded.'.format(self.stream.port))
		else:
			raise AlreadyConnectedError('{} is already connected'.format(self.port))

	def disconnect(self):
		if self.is_connected:
			self.is_connected = False

			# Stop the listening thread
			self.listener.stop.set()
			if self.listener is not current_thread():
				self.listener.join()

			# Close the socket
			self.stream.close()

	def rawsend(self, rawbytes):
		if not self.is_connected:
			raise NotConnectedError('\'{}\' is not connected.'.format(self.port))
		
		sentbytes = self.stream.write(rawbytes)
		return sentbytes
	
	def send(self, opcode, *args):
		content = bytes([opcode]) + bytes().join(args)
		prefix  = MASTER_BYTE + bytes([len(content)])
		sentbytes = self.rawsend(prefix + content)
		return sentbytes

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
		opcode = message[0]
		args   = message[1:]
		queue = self.get_queue(opcode)
		queue.put(args)

	def poll(self, opcode, timeout=0):
		queue = self.get_queue(opcode)
		block = (timeout is None or timeout > 0)
		try:
			rawbytes = queue.get(block, timeout)
			return Deserializer(rawbytes)
		except Empty:
			return None
	
	def flush(self, opcode):
		while self.poll(opcode) is not None:
			pass

	def execute(self, opcode, *args, timeout=1):
		self.flush(opcode)
		self.send(opcode, *args)
		output = self.poll(opcode, timeout)
		return output

	def getuuid(self, timeout=1):
		output = self.execute(GETUUID_OPCODE, timeout=timeout)
		if output is not None:
			return output.read(STRING)
		else:
			return None

	def setuuid(self, uuid):
		return self.send(SETUUID_OPCODE, STRING(uuid))

	def getlog(self, opcode, timeout=0):
		log = str()
		while True:
			try:
				log += self.poll(opcode, timeout).read(STRING)
			except AttributeError:
				return log

	def getout(self, timeout=0):
		return self.getlog(STDOUT_OPCODE, timeout)

	def geterr(self, timeout=0):
		return self.getlog(STDERR_OPCODE, timeout)


class SerialListener(Thread):

	def __init__(self, parent):
		Thread.__init__(self)
		self.parent = parent
		self.stop   = Event()
		self.daemon = True

	def run(self):
		state  = 'waiting' # ['waiting', 'starting', 'receiving']
		buffer = bytes()
		msglen = 0
		while not self.stop.is_set():
			# Wait until new bytes arrive
			try:
				inc = self.parent.stream.read()
			except serial.serialutil.SerialException:
				self.parent.disconnect()
				break

			# Finite state machine
			if state == 'waiting' and inc == SLAVE_BYTE:
				state = 'starting'
			
			elif state == 'starting' and inc:
				msglen = inc[0]
				state  = 'receiving'

			elif state == 'receiving':
				buffer += inc
				if (len(buffer) >= msglen):
					self.parent.process(buffer)
					buffer = bytes()
					state  = 'waiting'
