#!/usr/bin/python3
#-*- coding: utf-8 -*-

import socket
import os
import time
from threading import Thread, RLock, Event
import pickle

CLOSE_OPCODE = 10

class TCPTalks(Thread):

	def __init__(self, ip = None, readport = 25565, writeport = 25566):
		Thread.__init__(self)

		# Sockets
		self.serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.server = None
		self.client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

		self.selfip  = self.getip()
		self.otherip = ip

		if ip is None:
			self.readport  = writeport
			self.writeport = readport
		else:
			self.readport  = readport
			self.writeport = writeport

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

	def getip(self):
		if os.name == 'nt':
			return [ip for ip in socket.gethostbyname_ex(socket.gethostname())[2] if not ip.startswith("127.")][0]
		elif os.name == 'posix':
			a = os.popen("""ifconfig | awk '/inet adr/ {gsub("adr:", "", $2); print $2}'""").readlines()
			a.remove('127.0.0.1\n')
			return a[0][0:-1]

	def connect(self, timeout = None):
		# Raspberry Pi
		if self.otherip is None:

			# Create a server
			self.serversocket.bind((self.selfip, self.readport))
			self.serversocket.listen(1)

			# Wait for the other's ip
			self.serversocket.settimeout(timeout)
			self.server, self.otherip = self.serversocket.accept()
			self.otherip = self.otherip[0]

			# Connect to the other
			self.client.settimeout(timeout)
			self.client.connect((self.otherip, self.writeport))

		# PC (Windows or Linux)
		else:
			# Connect to the other
			t = time.time()
			while timeout is None or time.time() - t < timeout:
				try:
					self.client.connect((self.otherip, self.writeport))
				except socket.error:
					continue
				else:
					break
				#TODO renvoyer une exception si le timeout a été dépassé
			
			# Create a server
			self.serversocket.bind((self.selfip, self.readport))
			self.serversocket.listen(1)

			# Wait for the other
			self.serversocket.settimeout(timeout)
			self.server, self.otherip = self.serversocket.accept()

	def disconnect(self):
		try:
			self.send(CLOSE_OPCODE)
		except socket.error:
			pass
		self.serversocket.close()
		self.client.close()

	def is_connected(self):
		pass

	def bind(self, opcode, instruction):
		pass

	def send(self, opcode, *args):
		return self.server.send(pickle.dumps(['R', opcode] + list(args)))

	def sendback(self, opcode, *args):
		return self.server.send(pickle.dumps(['A', opcode] + list(args)))

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
			try:
				inc = self.client.recv(8192)
			except socket.error:
				continue
			self.process(pickle.loads(inc))

	def process(self, message):
		direction = message[0]
		opcode    = message[1]
		if (direction == 'A'):
			queue = self.get_queue(opcode)
			queue.put(message[2:])
		elif (direction == 'R'):
			pass #TODO executer la fonction associée à l'opcode' 

	def poll(self, opcode, timeout = 0):
		if not self.is_connected():
			raise RuntimeError('\'{}\' is not connected.'.format(self.otherip))
			
		queue = self.get_queue(opcode)
		block = (timeout is None or timeout > 0)
		try:
			return tuple(queue.get(block, timeout))
		except Empty:
			return None
	
	def flush(self, opcode):
		while self.poll(opcode) is not None:
			pass
