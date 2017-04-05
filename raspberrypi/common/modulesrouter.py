#!/usr/bin/env python3
#-*- coding: utf-8 -*-

import os

from common.tcptalks    import TCPTalks
from common.serialtalks import SerialTalks, AlreadyConnectedError, ConnectionFailedError, MuteError
try: from common.LightButton import Switch, LightButton
except ImportError: pass

MODULESROUTER_DEFAULT_PORT = 25566

MODULE_CONNECT_OPCODE = 0x10
MODULE_EXECUTE_OPCODE = 0x11
MODULE_GETATTR_OPCODE = 0x12
MODULE_SETATTR_OPCODE = 0x13

SWITCH_CONNECT_OPCODE = 0x20
SWITCH_EXECUTE_OPCODE = 0x21
SWITCH_GETATTR_OPCODE = 0x22
SWITCH_SETATTR_OPCODE = 0x23

BUTTON_CONNECT_OPCODE = 0x30
BUTTON_EXECUTE_OPCODE = 0x31
BUTTON_GETATTR_OPCODE = 0x32
BUTTON_SETATTR_OPCODE = 0x33

FUNCTION_EXECUTE_OPCODE = 0x40


# Exceptions

class ModuleError(KeyError): pass
class SwitchModuleError(KeyError): pass
class ButtonModuleError(KeyError): pass


# Main classes

class ModulesRouter(TCPTalks):

	def __init__(self, port=MODULESROUTER_DEFAULT_PORT, password=None):
		TCPTalks.__init__(self, port=port, password=password)
		self.bind(MODULE_CONNECT_OPCODE, self.module_connect)
		self.bind(MODULE_EXECUTE_OPCODE, self.module_execute)
		self.bind(MODULE_GETATTR_OPCODE, self.module_getattr)
		self.bind(MODULE_SETATTR_OPCODE, self.module_setattr)
		self.modules  = dict()
		self.buttons  = dict()
		self.switches = dict()
		
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

	def module_connect(self, uuid):
		self.module_setup(uuid)

	def module_execute(self, uuid, methodname, *args, **kwargs):
		module = self.module_setup(uuid)
		try:
			kwargs['timeout'] = kwargs['serialtimeout']
			del kwargs['serialtimeout']
		except KeyError: pass
		return getattr(module, methodname)(*args, **kwargs)

	def module_getattr(self, uuid, attrname):
		module = self.module_setup(uuid)
		return getattr(module, attrname)

	def module_setattr(self, uuid, attrname, attrvalue):
		module = self.module_setup(uuid)
		return setattr(module, attrname, attrvalue)

	def button_setup(self, butpin, ledpin):
		uuid = butpin
		try:
			button = self.buttons[uuid]
		except KeyError:
			button = LightButton(butpin, ledpin, function=self.send, FUNCTION_EXECUTE_OPCODE, uuid)
			self.buttons[uuid] = button
		return button

	def button_connect(self, uuid):
		self.button_setup(uuid)

	def button_execute(self, uuid, methodname, *args, **kwargs):
		button = self.button_setup(uuid)
		return getattr(button, methodname)(*args, **kwargs)

	def button_getattr(self, uuid, attrname):
		button = self.button_setup(uuid)
		return getattr(button, attrname)

	def button_setattr(self, uuid, attrname, attrvalue):
		button = self.button_setup(uuid)
		return setattr(button, attrname, attrvalue)

	def switch_connect(self, uuid):
		self.switch_setup(uuid)

	def switch_setup(self, pin):
		uuid = pin
		try:
			switch = self.switches[uuid]
		except KeyError:
			switch = Switch(pin, function=self.send, FUNCTION_EXECUTE_OPCODE, uuid)
			self.switches[uuid] = switch
		return switch

	def switch_execute(self, uuid, methodname, *args, **kwargs):
		switch = self.switch_setup(uuid)
		return getattr(switch, methodname)(*args, **kwargs)

	def switch_getattr(self, uuid, attrname):
		switch = self.switch_setup(uuid)
		return getattr(switch, attrname)

	def switch_setattr(self, uuid, attrname, attrvalue):
		switch = self.switch_setup(uuid)
		return setattr(switch, attrname, attrvalue)


class ModulesManager(TCPTalks):

	def __init__(self, ip='localhost', port=MODULESROUTER_DEFAULT_PORT, password=None):
		TCPTalks.__init__(self, ip, port=port, password=password)
		self.buttons_functions = dict()
		self.buttons_args      = dict()
		self.bind(FUNCTION_EXECUTE_OPCODE, self.execute_function)
	
	def execute_function(self, uuid):
		try:
			self.buttons_functions[uuid](*self.buttons_args[uuid])
		except IndexError:
			pass


class Module:

	def __init__(self, parent, uuid, timeout=2):
		self.parent = parent
		self.uuid   = uuid
		self.parent.execute(MODULE_CONNECT_OPCODE, uuid, timeout=timeout)

	def execmeth(self, methodname, *args, tcptimeout=60, **kwargs):
		# Rename the SerialTalks 'timeout' argument so that it doesn't conflict with the TCPTalks one
		try:
			kwargs['serialtimeout'] = kwargs['timeout']
			del kwargs['timeout']
		except KeyError: pass
		
		# Remotely call the SerialTalks method
		return self.parent.execute(MODULE_EXECUTE_OPCODE, self.uuid, methodname, *args, **kwargs, timeout=tcptimeout)

	def getattr(self, attrname):
		return self.parent.execute(MODULE_GETATTR_OPCODE, self.uuid, name)

	def setattr(self, attrname, value):
		return self.parent.execute(MODULE_SETATTR_OPCODE, self.uuid, name, value)

	# Shortcuts
	def connect   (self, *args, **kwargs): return self.execmeth('connect',    *args, **kwargs)
	def disconnect(self, *args, **kwargs): return self.execmeth('disconnect', *args, **kwargs)
	def send      (self, *args, **kwargs): return self.execmeth('send',       *args, **kwargs)
	def poll      (self, *args, **kwargs): return self.execmeth('poll',       *args, **kwargs)
	def flush     (self, *args, **kwargs): return self.execmeth('flush',      *args, **kwargs)
	def execute   (self, *args, **kwargs): return self.execmeth('execute',    *args, **kwargs)
	def getuuid   (self, *args, **kwargs): return self.execmeth('getuuid',    *args, **kwargs)
	def setuuid   (self, *args, **kwargs): return self.execmeth('setuuid',    *args, **kwargs)
	def getout    (self, *args, **kwargs): return self.execmeth('getout',     *args, **kwargs)
	def geterr    (self, *args, **kwargs): return self.execmeth('geterr',     *args, **kwargs)


class LightButtonModule:

	def __init__(self, parent, butpin, ledpin, timeout=2):
		self.parent = parent
		self.uuid   = butpin
		self.parent.execute(BUTTON_CONNECT_OPCODE, uuid, timeout=timeout)
	
	def execmeth(self, methodname, *args, tcptimeout=60, **kwargs):
		return self.parent.execute(BUTTON_EXECUTE_OPCODE, self.uuid, methodname, *args, **kwargs, timeout=tcptimeout)
	
	def getattr(self, attrname):
		return self.parent.execute(BUTTON_GETATTR_OPCODE, self.uuid, name)
	
	def setattr(self, attrname, value):
		return self.parent.execute(BUTTON_SETATTR_OPCODE, self.uuid, name, value)
	
	def SetFunction(self, function, *args):
		self.parent.buttons_functions[self.uuid] = function
		self.parent.args_functions   [self.uuid] = args

	def SetAutoSwitch(self, *args, **kwargs): return self.execmeth('SetAutoSwitch', *args, **kwargs)
	def On           (self, *args, **kwargs): return self.execmeth('On',            *args, **kwargs)
	def Off          (self, *args, **kwargs): return self.execmeth('Off',           *args, **kwargs)
	def Switch       (self, *args, **kwargs): return self.execmeth('Switch',        *args, **kwargs)
	def Close        (self, *args, **kwargs): return self.execmeth('Close',         *args, **kwargs)

