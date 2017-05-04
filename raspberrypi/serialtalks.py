#!/usr/bin/python3
#-*- coding: utf-8 -*-

import sys
import serial
from serial.serialutil import SerialException
import time
import random
from queue		import Queue, Empty
from threading	import Thread, RLock, Event, current_thread

from serialutils import Deserializer, IntegerType, FloatType, StringType

BAUDRATE = 115200

MASTER_BYTE = b'R'
SLAVE_BYTE  = b'A'

PING_OPCODE    = 0x00
GETUUID_OPCODE = 0x01
SETUUID_OPCODE = 0x02
STDOUT_RETCODE = 0xFFFFFFFF
STDERR_RETCODE = 0xFFFFFFFE

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

	def connect(self, timeout=5):
		if self.is_connected:
			raise AlreadyConnectedError('{} is already connected'.format(self.port))
		
		# Connect to the serial port
		try:
			self.stream = serial.Serial(self.port, baudrate=BAUDRATE, bytesize=serial.EIGHTBITS, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE)
			self.stream.timeout	= 1
		except SerialException as e:
			raise ConnectionFailedError(str(e)) from None
		
		# Create a listening thread that will wait for inputs
		self.listener = SerialListener(self)
		self.listener.start()
		
		# Wait until the Arduino is operational
		startingtime = time.monotonic()
		while not self.is_connected:
			try:
				output = self.execute(PING_OPCODE, timeout=0.1)
			except NotConnectedError: pass
			except TimeoutError:
				if time.monotonic() - startingtime > timeout:
					self.disconnect()
					raise MuteError('\'{}\' is mute. It may not be an Arduino or it\'s sketch may not be correctly loaded.'.format(self.stream.port)) from None
				else:
					continue
			self.is_connected = True
			self.reset_queues()
			
	def disconnect(self):
		# Stop the listening thread
		if hasattr(self, 'listener') and self.listener.is_alive():
			self.listener.stop.set()
			if self.listener is not current_thread():
				self.listener.join()

		# Close the serial port
		if hasattr(self, 'stream') and self.stream.is_open:
			self.stream.close()
		
		# Unset the connected flag
		self.is_connected = False

	def rawsend(self, rawbytes):
		try:
			if hasattr(self, 'stream') and self.stream.is_open:
				sentbytes = self.stream.write(rawbytes)
				return sentbytes
		except SerialException: pass
		raise NotConnectedError('\'{}\' is not connected.'.format(self.port)) from None
	
	def send(self, opcode, *args):
		retcode = random.randint(0, 0xFFFFFFFF)
		content = BYTE(opcode) + ULONG(retcode) + bytes().join(args)
		prefix  = MASTER_BYTE + BYTE(len(content))
		self.rawsend(prefix + content)
		return retcode

	def get_queue(self, retcode):
		self.queues_lock.acquire()
		try:
			queue = self.queues_dict[retcode]
		except KeyError:
			queue = self.queues_dict[retcode] = Queue()
		finally:
			self.queues_lock.release()
		return queue

	def delete_queue(self, retcode):
		self.queues_lock.acquire()
		try:
			del self.queues_dict[retcode]
		finally:
			self.queues_lock.release()

	def reset_queues(self):
		self.queues_lock.acquire()
		self.queues_dict = dict()
		self.queues_lock.release()

	def process(self, message):
		retcode = message.read(ULONG)
		queue = self.get_queue(retcode)
		queue.put(message)

	def poll(self, retcode, timeout=0):
		queue = self.get_queue(retcode)
		block = (timeout is None or timeout > 0)
		try:
			output = queue.get(block, timeout)
		except Empty:
			if block:
				raise TimeoutError('timeout exceeded') from None
			else:
				return None
		if queue.qsize() == 0:
			self.delete_queue(retcode)
		return output
	
	def flush(self, retcode):
		while self.poll(retcode) is not None:
			pass

	def execute(self, opcode, *args, timeout=5):
		retcode = self.send(opcode, *args)
		output = self.poll(retcode, timeout)
		return output

	def getuuid(self, timeout=5):
		output = self.execute(GETUUID_OPCODE, timeout=timeout)
		return output.read(STRING)
		
	def setuuid(self, uuid):
		return self.send(SETUUID_OPCODE, STRING(uuid))

	def getlog(self, retcode, timeout=0):
		log = str()
		while True:
			output = self.poll(retcode, 0)
			if output is not None:
				log += output.read(STRING)
			else:
				output = self.poll(retcode, timeout)
				try:
					log += output.read(STRING)
				except TimeoutError:
					pass
		return log

	def getout(self, timeout=0):
		return self.getlog(STDOUT_RETCODE, timeout)

	def geterr(self, timeout=0):
		return self.getlog(STDERR_RETCODE, timeout)


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
				continue
			
			elif state == 'starting' and inc:
				msglen = inc[0]
				state  = 'receiving'
				continue

			elif state == 'receiving':
				buffer += inc
				if (len(buffer) < msglen):
					continue
			
			# Junk byte
			else: continue
			
			# Process the above message
			try:
				self.parent.process(Deserializer(buffer))
			except NotConnectedError:
				self.disconnect()
				break
			
			# Reset the finite state machine
			state  = 'waiting'
			buffer = bytes()
