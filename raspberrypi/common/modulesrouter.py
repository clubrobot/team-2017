#!/usr/bin/env python3
#-*- coding: utf-8 -*-

import os

from common.tcptalks    import TCPTalks
from common.serialtalks import SerialTalks, AlreadyConnectedError, ConnectionFailedError, MuteError

MODULESROUTER_DEFAULT_PORT = 25566

MODULE_SETUP_OPCODE   = 0x10
MODULE_EXECUTE_OPCODE = 0x11
MODULE_GETATTR_OPCODE = 0x12
MODULE_SETATTR_OPCODE = 0x13


# Exceptions

class ModuleError(KeyError): pass


# Main classes

class ModulesRouter(TCPTalks):

	def __init__(self, port=MODULESROUTER_DEFAULT_PORT, password=None):
		TCPTalks.__init__(self, port=port, password=password)
		self.bind(MODULE_SETUP_OPCODE,   self.module_setup)
		self.bind(MODULE_EXECUTE_OPCODE, self.module_execute)
		self.bind(MODULE_GETATTR_OPCODE, self.module_getattr)
		self.bind(MODULE_SETATTR_OPCODE, self.module_setattr)
		self.modules = dict()

	def disconnect(self):
		TCPTalks.disconnect(self)
		for module in self.modules.values():
			module.disconnect()

	def module_setup(self, uuid):
		# Create the associated physical SerialTalks instance if it doesn't exist yet
		try:
			module = self.modules[uuid]
		except KeyError:
			module = SerialTalks(os.path.join('/dev/arduino', uuid))
			self.modules[uuid] = module
		
		# Try to connect to the Arduino (it may have been unplugged)
		try:
			module.connect()
		except AlreadyConnectedError: pass
		except (ConnectionFailedError, MuteError):
			raise ModuleError('modules router has no module \'{}\''.format(uuid))		

		return module

	def module_execute(self, uuid, methodname, *args, **kwargs):
		module = self.module_setup(uuid)
		return getattr(module, methodname)(*args, **kwargs)

	def module_getattr(self, uuid, attrname):
		module = self.module_setup(uuid)
		return getattr(module, attrname)

	def module_setattr(self, uuid, attrname, attrvalue):
		module = self.module_setup(uuid)
		return setattr(module, attrname, attrvalue)


class ModulesManager(TCPTalks):

	def __init__(self, ip='localhost', port=MODULESROUTER_DEFAULT_PORT, password=None):
		TCPTalks.__init__(self, ip, port=port, password=password)


class Module:

	def __init__(self, parent, uuid, timeout=2):
		self.parent = parent
		self.uuid   = uuid
		self.parent.execute(MODULE_SETUP_OPCODE, uuid, timeout=timeout)

	def __getattr__(self, name):
		methods = (
			'disconnect',
			'send', 'poll', 'flush', 'execute',
			'getuuid', 'setuuid', 'getout', 'geterr')
		attributes = ('port', 'is_connected')
		if name in methods:
			def temporary_method(*args, **kwargs):
				return self.parent.execute(MODULE_EXECUTE_OPCODE, self.uuid, name, *args, **kwargs)
			return temporary_method
		elif name in attributes:
			return self.parent.execute(MODULE_GETATTR_OPCODE, self.uuid, name)
		else:
			return object.__getattribute__(self, name)

	def __setattr__(self, name, value):
		attributes = ('port',)
		if name in attributes:
			return self.parent.execute(MODULE_SETATTR_OPCODE, self.uuid, name, value)
		else:
			return object.__setattr__(self, name, value)
