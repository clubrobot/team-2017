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

SWITCH_CREATE_OPCODE  = 0x20
SWITCH_EXECUTE_OPCODE = 0x21
SWITCH_GETATTR_OPCODE = 0x22
SWITCH_SETATTR_OPCODE = 0x23

BUTTON_CREATE_OPCODE  = 0x30
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
		self.bind(SWITCH_CREATE_OPCODE,  self.switch_create)
		self.bind(SWITCH_EXECUTE_OPCODE, self.switch_execute)
		self.bind(SWITCH_GETATTR_OPCODE, self.switch_getattr)
		self.bind(SWITCH_SETATTR_OPCODE, self.switch_setattr)
		self.bind(BUTTON_CREATE_OPCODE,  self.button_create)
		self.bind(BUTTON_EXECUTE_OPCODE, self.button_execute)
		self.bind(BUTTON_GETATTR_OPCODE, self.button_getattr)
		self.bind(BUTTON_SETATTR_OPCODE, self.button_setattr)
		self.modules  = dict()
		self.buttons  = dict()
		self.switches = dict()
		
	def disconnect(self):
		TCPTalks.disconnect(self)
		for module in self.modules.values():
			module.disconnect()
		for button in self.buttons.values():
			button.Close()
		for swicth in self.switches.values():
			switch.Close()

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

	def button_create(self, butpin, ledpin):
		uuid = butpin
		try:
			button = LightButton(butpin, ledpin, self.send, FUNCTION_EXECUTE_OPCODE, uuid)
			self.buttons[uuid] = button
		except RuntimeError: pass

	def button_execute(self, uuid, methodname, *args, **kwargs):
		button = self.buttons[uuid]
		return getattr(button, methodname)(*args, **kwargs)

	def button_getattr(self, uuid, attrname):
		button = self.buttons[uuid]
		return getattr(button, attrname)

	def button_setattr(self, uuid, attrname, attrvalue):
		button = self.buttons[uuid]
		return setattr(button, attrname, attrvalue)

	def switch_create(self, pin):
		uuid = pin
		try:
			switch = Switch(pin, self.send, FUNCTION_EXECUTE_OPCODE, uuid)
			self.switches[uuid] = switch
		except RuntimeError: pass

	def switch_execute(self, uuid, methodname, *args, **kwargs):
		switch = self.switches[uuid]
		return getattr(switch, methodname)(*args, **kwargs)

	def switch_getattr(self, uuid, attrname):
		switch = self.switches[uuid]
		return getattr(switch, attrname)

	def switch_setattr(self, uuid, attrname, attrvalue):
		switch = self.switches[uuid]
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
		self.parent.execute(BUTTON_CREATE_OPCODE, butpin, ledpin, timeout=timeout)
	
	def execmeth(self, methodname, *args, tcptimeout=60, **kwargs):
		return self.parent.execute(BUTTON_EXECUTE_OPCODE, self.uuid, methodname, *args, **kwargs, timeout=tcptimeout)
	
	def getattr(self, attrname):
		return self.parent.execute(BUTTON_GETATTR_OPCODE, self.uuid, name)
	
	def setattr(self, attrname, value):
		return self.parent.execute(BUTTON_SETATTR_OPCODE, self.uuid, name, value)
	
	def SetFunction(self, function, *args):
		self.parent.buttons_functions[self.uuid] = function
		self.parent.buttons_args     [self.uuid] = args

	def SetAutoSwitch(self, *args, **kwargs): return self.execmeth('SetAutoSwitch', *args, **kwargs)
	def On           (self, *args, **kwargs): return self.execmeth('On',            *args, **kwargs)
	def Off          (self, *args, **kwargs): return self.execmeth('Off',           *args, **kwargs)
	def Switch       (self, *args, **kwargs): return self.execmeth('Switch',        *args, **kwargs)
	def Close        (self, *args, **kwargs): return self.execmeth('Close',         *args, **kwargs)


class SwitchModule:

	def __init__(self, parent, pin, timeout=2):
		self.parent = parent
		self.uuid   = pin
		self.parent.execute(SWITCH_CREATE_OPCODE, pin, timeout=timeout)
	
	def execmeth(self, methodname, *args, tcptimeout=60, **kwargs):
		return self.parent.execute(SWITCH_EXECUTE_OPCODE, self.uuid, methodname, *args, **kwargs, timeout=tcptimeout)
	
	def getattr(self, attrname):
		return self.parent.execute(SWITCH_GETATTR_OPCODE, self.uuid, name)
	
	def setattr(self, attrname, value):
		return self.parent.execute(SWITCH_SETATTR_OPCODE, self.uuid, name, value)
	
	def SetFunction(self, function, *args):
		self.parent.switches_functions[self.uuid] = function
		self.parent.switches_args     [self.uuid] = args

	def Close(self, *args, **kwargs): return self.execmeth('Close', *args, **kwargs)
