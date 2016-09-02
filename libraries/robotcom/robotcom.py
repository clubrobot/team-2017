#!/usr/bin/python3
#-*- coding: utf-8 -*-

import serial
from queue		import Queue, Empty
from threading	import Thread, RLock, Event

BAUDRATE			= 115200

COMMAND_START_BYTE	= b'R'
OUTPUT_START_BYTE	= b'A'

GET_UUID_OPCODE		= 0x00
SET_UUID_OPCODE		= 0x01

class RobotCom:

	def __init__(self, port):
		self.serial			= serial.Serial(baudrate	= BAUDRATE,
											bytesize	= serial.EIGHTBITS,
											parity		= serial.PARITY_NONE,
											stopbits	= serial.STOPBITS_ONE)
		self.serial.port	= port
		self.serial.timeout	= 1

	def connect(self, timeout = 2):
		self.serial.open()
		self.daemon = Daemon(self.serial)
		self.daemon.daemon = True
		self.daemon.start()
		if self.poll(GET_UUID_OPCODE, timeout) is None:
			raise TimeoutError('\'{}\' is mute. It may not be an Arduino or it\'s sketch may not be correctly loaded.'.format(self.serial.port))

	def disconnect(self):
		self.daemon.stop_event.set()
		self.daemon.join()
		self.serial.close()

	def send(self, opcode, *params):
		prefix = COMMAND_START_BYTE
		content = bytes([opcode])

		for param in params:
			if isinstance(param, bytes):
				content += param
			elif isinstance(param, str):
				content += bytes(param, 'charmap')
			elif isinstance(param, int):
				content += bytes([param])
			elif isinstance(param, float):
				content += bytes([round(param)])
			else:
				content += bytes(param)

		prefix += bytes([len(content)])
		return self.serial.write(prefix + content)
	
	def poll(self, opcode, timeout = 0):
		queue = self.daemon.get_queue(opcode)
		block = (timeout is None or timeout > 0)
		try:
			return queue.get(block, timeout)
		except Empty:
			return None
	
	def flush(self, opcode):
		while self.poll(opcode) is not None:
			pass
	
	def get_uuid(self, timeout = 1):
		self.flush(GET_UUID_OPCODE)
		self.send(GET_UUID_OPCODE)
		uuid = self.poll(GET_UUID_OPCODE, timeout)
		if uuid is not None:
			return uuid.decode('charmap')
		else:
			return None

	def set_uuid(self, uuid):
		return self.send(SET_UUID_OPCODE, uuid)

	def __enter__(self):
		self.connect()
		return self

	def __exit__(self, exc_type, exc_value, traceback):
		self.disconnect()

class Daemon(Thread):

	def __init__(self, serial):
		Thread.__init__(self)
		self.serial			= serial
		self.queues_dict	= dict()
		self.queues_lock	= RLock()
		self.stop_event		= Event()

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
		raw_message = b''

		while True:
			byte = self.serial.read()
			if byte == OUTPUT_START_BYTE or len(raw_message) > 0:
				raw_message += byte
			elif self.stop_event.is_set():
				break

			if len(raw_message) >= 2:
				message			= raw_message[2:]
				message_length	= raw_message[1]
				if message_length <= len(message):
					if message:
						queue = self.get_queue(message[0])
						queue.put(message[1:])
					raw_message = b''
				# if message_length <= len(message):
			# if len(raw_message) >= 2:
		# while not self.stop_event.is_set():

if __name__ == '__main__':
	from pprint import pprint

	with RobotCom('/dev/ttyUSB0') as ard0, RobotCom('/dev/ttyUSB1') as ard1:
		print(ard0.get_uuid())
		print(ard1.get_uuid())