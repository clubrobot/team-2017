#!/usr/bin/env python3
#-*- coding: utf-8 -*-

import glob
from serial.serialutil import SerialException

from tcptalks    import TCPTalks
from serialtalks import SerialTalks

MODULECONNECT_OPCODE = 0x10
MODULEEXECUTE_OPCODE = 0x11


class ModulesRouter(TCPTalks):

	def __init__(self):
		TCPTalks.__init__(self)
		self.bind(MODULECONNECT_OPCODE, self.moduleconnect)
		self.bind(MODULEEXECUTE_OPCODE, self.moduleexecute)
		self.modules = dict()

	def moduleconnect(self, uuid, timeout):
		if not uuid in self.modules or not self.modules[uuid].is_connected:

			# We try to connect to every available ttyUSB*
			for tty in glob.iglob('/dev/ttyUSB*'):
				try:
					module = SerialTalks(tty)
					module.connect(timeout)
				except (TimeoutError, SerialException):
					continue

				# Return if we found the right Arduino
				if module.getuuid() == uuid:
					self.modules[uuid] = module
					return
				
				# Else disconnect it and continue
				module.disconnect()
			
			# Raise an error if we didn't found any Arduino
			raise RuntimeError('module \'{}\' is not connected or is not responding'.format(uuid))


	def moduleexecute(self, uuid, methodname, *args, **kwargs):
		try:
			module = self.modules[uuid]
		except KeyError:
			raise RuntimeError('modules router has no module \'{}\''.format(uuid)) from None
		return getattr(module, methodname)(*args, **kwargs)


class Module:

	def __init__(self, parent, uuid, timeout = 2):
		self.parent = parent
		self.uuid   = uuid
		self.parent.execute(MODULECONNECT_OPCODE, uuid, timeout)

	def __getattr__(self, methodname):
		def method(*args, **kwargs):
			return self.parent.execute(MODULEEXECUTE_OPCODE, self.uuid, methodname, *args, **kwargs)
		return method


if __name__ == '__main__':
	router = ModulesRouter()
	while True:
		try:
			router.connect(None)
		except RuntimeError:
			pass
		except KeyboardInterrupt:
			break
