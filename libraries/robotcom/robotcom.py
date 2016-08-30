#!/usr/bin/python3
#-*- coding: utf-8 -*-

import serial
from queue		import Queue, Empty
from threading	import Thread, RLock, Event

BAUDRATE			= 115200

COMMAND_START_BYTE	= b'R'
OUTPUT_START_BYTE	= b'A'

UUID_GET_OPCODE		= 0x00
UUID_SET_OPCODE		= 0x01

class RobotCom:

	def __init__(self, port):
		self.serial			= serial.Serial(baudrate = BAUDRATE)
		self.serial.port	= port

	def connect(self):
		self.serial.open()
		self.daemon = Daemon(self.serial)
		self.daemon.daemon = True
		self.daemon.start()
	
	def disconnect(self):
		self.serial.close()
		self.daemon.stop_event.set()
		self.daemon.join() # Is this needed?

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
		self.serial.write(prefix)
		self.serial.write(content)
	
	def poll(self, opcode, timeout = 0):
		queue = self.daemon.get_queue(opcode)
		block = (timeout is None or timeout > 0)
		try:
			return queue.get(block, timeout)
		except Empty:
			return None
	
	def get_uuid(self, timeout = 1):
		self.send(UUID_GET_OPCODE)
		uuid = self.poll(UUID_GET_OPCODE, timeout)
		if uuid is not None:
			return uuid.decode('charmap')
		else:
			return None

	def set_uuid(self, uuid):
		self.send(UUID_SET_OPCODE, uuid)

	def __enter__(self):
		self.connect()
		return self

	def __exit__(self):
		self.disconnect()

class Daemon(Thread):

	def __init__(self, serial):
		Thread.__init__(self)
		self.serial			= serial
		self.serial.timeout	= 1 # 1 second timeout
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

		while not self.stop_event.is_set():
			byte = self.serial.read()
			if byte == OUTPUT_START_BYTE or len(raw_message) > 0:
				raw_message += byte

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
