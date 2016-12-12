#!/usr/bin/env python3
#-*- coding: utf-8 -*-

import glob

from tcptalks import TCPTalks

NEWMODULE_OPCODE  = 0x10
MODULESEND_OPCODE = 0x11
MODULEPOLL_OPCODE = 0x12
MODULEEXEC_OPCODE = 0x13


class ModulesRouterServer(TCPTalks):

	def __init__(self):
		TCPTalks.__init__(self)

		self.bind(NEWMODULE_OPCODE , self.newmodule)
		self.bind(MODULESEND_OPCODE, self.modulesend)
		self.bind(MODULEPOLL_OPCODE, self.modulepoll)
		self.bind(MODULEEXEC_OPCODE, self.moduleexec)
		
		self.modules = dict()

	def newmodule(self, uuid, timeout):
		for tty in glob.iglob('/dev/ttyUSB*'): # We try to connect to every available ttyUSB*
			# This will only pass if the current Arduino is a valid SerialTalks instance
			try:
				module = SerialTalks(tty)
				module.connect(timeout)
			except TimeoutError: # Add bad serial connection error
				continue

			# Return if we found the right Arduino
			if module.getuuid() == uuid:
				self.modules[uuid] = module
				return uuid

			# Else disconnect it and continue
			module.disconnect()
		
		# Raise an error if we didn't found any Arduino
		raise RuntimeError('module \'{}\' is not connected or is not responding')

	def getmodule(self, uuid):
		try:
			module = self.modules[uuid]
		except KeyError:
			raise RuntimeError('modules router has no module \'{}\''.format(uuid)) from None
		
		# Reconnect module if needed
		if not module.is_connected:
			module.connect(None)
		
		return module

	def modulesend(self, uuid, opcode, args):
		return self.getmodule().send(opcode, args)
	
	def modulepoll(self, uuid, opcode, timeout):
		return self.getmodule().poll(opcode, timeout)

	def moduleexec(self, uuid, opcode, args):
		module = self.getmodule()
		module.flush(opcode)
		module.send(opcode, args)
		return module.poll(opcode, None)


class ModulesRouter(TCPTalks):

	def __init__(self, ip):
		TCPTalks.__init__(self, ip)

	def newmodule(self, uuid, timeout = 2):
		self.execute(NEWMODULE_OPCODE, uuid, timeout)
		return Module(self, uuid)


class Module:

	def __init__(self, parent, uuid):
		self.parent = parent
		self.uuid   = uuid
	
	def send(self, opcode, args):
		return self.parent.send(MODULESEND_OPCODE, self.uuid, opcode, args)
	
	def poll(self, uuid, opcode, timeout):
		return self.parent.execute(MODULEPOLL_OPCODE, self.uuid, opcode, timeout)

	def execute(self, uuid, opcode, args):
		return self.parent.execute(MODULEEXEC_OPCODE, self.uuid, opcode, args)


if __name__ == '__main__':
	server = ModulesRouterServer()
	while True:
		try:
			server.connect(None)
		except RuntimeError:
			pass
