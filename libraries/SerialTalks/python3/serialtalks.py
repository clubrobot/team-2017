#!/usr/bin/python3
#-*- coding: utf-8 -*-

import serial
from queue		import Queue, Empty
from threading	import Thread, RLock, Event

from serialtalks import serialutils

BAUDRATE = 115200

MASTER_BYTE = b'R'
SLAVE_BYTE  = b'A'

GETUUID_OPCODE = 0x00
SETUUID_OPCODE = 0x01
STDOUT_OPCODE  = 0xFF
STDERR_OPCODE  = 0xFE

BYTEORDER = 'little'
ENCODING  = 'utf-8'

CHAR   = serialutils.IntegerType(1, BYTEORDER, True)
UCHAR  = serialutils.IntegerType(1, BYTEORDER, False)
SHORT  = serialutils.IntegerType(2, BYTEORDER, True)
USHORT = serialutils.IntegerType(2, BYTEORDER, False)
LONG   = serialutils.IntegerType(4, BYTEORDER, True)
ULONG  = serialutils.IntegerType(4, BYTEORDER, False)

FLOAT  = serialutils.FloatType('f')

STRING = serialutils.StringType(ENCODING)

BYTE   = UCHAR
INT    = SHORT
UINT   = USHORT
DOUBLE = FLOAT


class SerialTalks(Thread):

	def __init__(self, port):
		Thread.__init__(self)

		# Serial things
		self.stream			= serial.Serial(baudrate	= BAUDRATE,
											bytesize	= serial.EIGHTBITS,
											parity		= serial.PARITY_NONE,
											stopbits	= serial.STOPBITS_ONE)
		self.stream.port	= port
		self.stream.timeout	= 1

		# Finite state machine
		self.state       = 'waiting' # ['waiting', 'starting', 'receiving']
		self.buffer      = b''
		self.bytesnumber = 0

		# Threading things
		self.daemon      = True
		self.queues_dict = dict()
		self.queues_lock = RLock()
		self.stop_event  = Event()

	def __enter__(self):
		self.connect()
		return self

	def __exit__(self, exc_type, exc_value, traceback):
		self.disconnect()

	def connect(self, timeout = 2):
		self.stream.open()
		self.start()
		if self.getuuid(timeout) is None:
			self.disconnect()
			raise TimeoutError('\'{}\' is mute. It may not be an Arduino or it\'s sketch may not be correctly loaded.'.format(self.stream.port))

	def disconnect(self):
		self.stop_event.set()
		self.join()
		self.stream.close()

	def is_connected(self):
		return self.stream.is_open # Needs Python 3.5 or later

	def send(self, opcode, *params):
		if not self.is_connected():
			raise RuntimeError('\'{}\' is not connected.'.format(self.stream.port))

		content  = bytes([opcode])
		content += b''.join(params)
		prefix   = MASTER_BYTE + bytes([len(content)])

		return self.stream.write(prefix + content)
	
	def get_queue(self, opcode):
		self.queues_lock.acquire()
		try:
			queue = self.queues_dict[opcode]
		except KeyError:
			queue = self.queues_dict[opcode] = Queue()
		finally:
			self.queues_lock.release()
		return queue

	def run(self):
		while not self.stop_event.is_set():
			inc = self.stream.read()

			if self.state == 'waiting' and inc == SLAVE_BYTE :
				self.state = 'starting'
			
			elif self.state == 'starting':
				self.bytesnumber = inc[0]
				self.state       = 'receiving'

			elif self.state == 'receiving':
				self.buffer += inc
				if (len(self.buffer) >= self.bytesnumber):
					self.process(self.buffer)
					self.buffer = b''
					self.state  = 'waiting'

	def process(self, message):
		opcode = message[0]
		queue = self.get_queue(opcode)
		queue.put(message[1:])

	def poll(self, opcode, timeout = 0):
		if not self.is_connected():
			raise RuntimeError('\'{}\' is not connected.'.format(self.stream.port))
			
		queue = self.get_queue(opcode)
		block = (timeout is None or timeout > 0)
		try:
			rawbytes = queue.get(block, timeout)
			return serialutils.Deserializer(rawbytes)
		except Empty:
			return None
	
	def flush(self, opcode):
		while self.poll(opcode) is not None:
			pass

	def getuuid(self, timeout = None):
		self.flush(GETUUID_OPCODE)
		self.send(GETUUID_OPCODE)
		uuid = self.poll(GETUUID_OPCODE, timeout).read(STRING)
		if uuid is not None:
			return uuid
		else:
			return None

	def setuuid(self, uuid):
		return self.send(SETUUID_OPCODE, STRING(uuid))

	def getlog(self, opcode, timeout = 0):
		log = str()
		while True:
			try:
				log += self.poll(opcode, timeout).read(STRING)
			except AttributeError:
				break
		return log

	def getout(self, timeout = 0):
		return self.getlog(STDOUT_OPCODE, timeout)

	def geterr(self, timeout = 0):
		return self.getlog(STDERR_OPCODE, timeout)

if __name__ == '__main__':
	from pprint import pprint

	ard0 = SerialTalks('/dev/ttyUSB0')
	#ard1 = SerialTalks('/dev/ttyUSB1')
	with ard0:
		print('UUID is:', ard0.getuuid())
		print('stdout:', ard0.getout(), end = '')
		print('stderr:', ard0.geterr(), end = '')
		#print(ard1.getuuid())
