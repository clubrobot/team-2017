#!/usr/bin/env python3
#-*- coding: utf-8 -*-

import os
from types import MethodType

from tcptalks import TCPTalks

COMPONENTS_SERVER_DEFAULT_PORT = 25566

CREATE_SERIALTALKS_COMPONENT_OPCODE = 0x10
CREATE_SWITCH_COMPONENT_OPCODE      = 0x11
CREATE_LIGHTBUTTON_COMPONENT_OPCODE = 0x12
CREATE_PICAMERA_COMPONENT_OPCODE    = 0x13

MAKE_COMPONENT_EXECUTE_OPCODE       = 0x20
GET_COMPONENT_ATTRIBUTE_OPCODE      = 0x21
SET_COMPONENT_ATTRIBUTE_OPCODE      = 0x22

UPDATE_MANAGER_PICAMERA_OPCODE      = 0x30
MAKE_MANAGER_EXECUTE_OPCODE         = 0x50


class Component():

	def _setup(self): pass
	def _cleanup(self): pass

try:
	from serialtalks import SerialTalks, AlreadyConnectedError
	
	class SerialTalksComponent(SerialTalks, Component):

		def __init__(self, uuid):
			SerialTalks.__init__(self, os.path.join('/dev/arduino', uuid))
		
		def _setup(self):
			try:
				self.connect()
			except AlreadyConnectedError:
				pass
		
		def _cleanup(self):
			self.disconnect()
except ImportError:
	pass

try:
	from gpiodevices import Switch, LightButton

	class SwitchComponent(Switch, Component):

		def __init__(self, switchpin):
			Switch.__init__(self, switchpin)
		
		def _cleanup(self):
			self.Close()

	class LightButtonComponent(LightButton, Component):

		def __init__(self, switchpin, ledpin):
			LightButton.__init__(self, switchpin, ledpin)
		
		def _cleanup(self):
			self.Close()
except ImportError:
	pass

try:
	from picamera import PiCamera
	from threading import Thread, Event
	import time
	import io

	class PiCameraComponent(PiCamera, Component):

		def __init__(self, server, resolution, framerate):
			PiCamera.__init__(self, resolution=resolution, framerate=framerate)
			self.server = server
			self.compid = None
			self.capturing = Event()
		
		def generate_streams_and_send_their_values(self, server, compid):
			stream = io.BytesIO()
			while self.capturing.is_set():
				# Give the hand to the PiCamera.capture_sequence method
				stream.seek(0)
				yield stream
				# Send the captured stream
				stream.truncate()
				try:
					server.send(UPDATE_MANAGER_PICAMERA_OPCODE, compid, stream.getvalue())
				except:
					break

		def start_capture(self):
			if not self.capturing.is_set():
				self.capturing.set()
				streams_generator = self.generate_streams_and_send_their_values(self.server, self.compid)
				# Start the capture in a thread
				target = self.capture_sequence
				args   = (streams_generator, 'jpeg')
				kwargs = {'use_video_port': True}
				thread = Thread(target=target, args=args, kwargs=kwargs, daemon=True)
				thread.start()
		
		def stop_capture(self):
			self.capturing.clear()

		def _cleanup(self):
			self.stop_capture()
except ImportError:
	pass

class Server(TCPTalks):

	def __init__(self, port=COMPONENTS_SERVER_DEFAULT_PORT, password=None):
		TCPTalks.__init__(self, port=port, password=password)
		self.bind(CREATE_SERIALTALKS_COMPONENT_OPCODE, self.CREATE_SERIALTALKS_COMPONENT)
		self.bind(CREATE_SWITCH_COMPONENT_OPCODE,      self.CREATE_SWITCH_COMPONENT)
		self.bind(CREATE_LIGHTBUTTON_COMPONENT_OPCODE, self.CREATE_LIGHTBUTTON_COMPONENT)
		self.bind(CREATE_PICAMERA_COMPONENT_OPCODE,    self.CREATE_PICAMERA_COMPONENT)
		self.bind(MAKE_COMPONENT_EXECUTE_OPCODE,       self.MAKE_COMPONENT_EXECUTE)
		self.bind(GET_COMPONENT_ATTRIBUTE_OPCODE,      self.GET_COMPONENT_ATTRIBUTE)
		self.bind(SET_COMPONENT_ATTRIBUTE_OPCODE,      self.SET_COMPONENT_ATTRIBUTE)
		self.components = {}
	
	def disconnect(self):
		TCPTalks.disconnect(self)
		self.cleanup()
		
	def cleanup(self):
		for comp in self.components.values():
			comp._cleanup()
		self.components = {}

	def addcomponent(self, comp):
		comp._setup()
		compid = id(comp)
		self.components[compid] = comp
		return compid

	def MAKE_COMPONENT_EXECUTE(self, compid, methodname, args, kwargs):
		comp = self.components[compid]
		comp._setup()
		return getattr(comp, methodname)(*args, **kwargs)

	def GET_COMPONENT_ATTRIBUTE(self, compid, attrname):
		comp = self.components[compid]
		comp._setup()
		return getattr(comp, attrname)

	def SET_COMPONENT_ATTRIBUTE(self, compid, attrname, attrvalue):
		comp = self.components[compid]
		comp._setup()
		return setattr(comp, attrname, attrvalue)

	def CREATE_SERIALTALKS_COMPONENT(self, uuid):
		comp = SerialTalksComponent(uuid)
		return self.addcomponent(comp)

	def CREATE_SWITCH_COMPONENT(self, switchpin):
		comp = SwitchComponent(switchpin)
		compid = self.addcomponent(comp)
		comp.SetFunction(self.send, MAKE_MANAGER_EXECUTE_OPCODE, compid)
		return compid

	def CREATE_LIGHTBUTTON_COMPONENT(self, switchpin, ledpin):
		comp = LightButtonComponent(switchpin, ledpin)
		compid = self.addcomponent(comp)
		comp.SetFunction(self.send, MAKE_MANAGER_EXECUTE_OPCODE, compid)
		return compid

	def CREATE_PICAMERA_COMPONENT(self, resolution, framerate):
		comp = PiCameraComponent(self, resolution, framerate)
		compid = self.addcomponent(comp)
		comp.server = self
		comp.compid = compid
		return compid

class Manager(TCPTalks):

	def __init__(self, ip='localhost', port=COMPONENTS_SERVER_DEFAULT_PORT, password=None):
		TCPTalks.__init__(self, ip, port=port, password=password)
		# PiCamera components
		self.bind(UPDATE_MANAGER_PICAMERA_OPCODE, self.UPDATE_MANAGER_PICAMERA)
		self.picameras = {}
		# GPIO components
		self.bind(MAKE_MANAGER_EXECUTE_OPCODE,    self.MAKE_MANAGER_EXECUTE)
		self.functions = {}
		self.args      = {}

	def UPDATE_MANAGER_PICAMERA(self, compid, streamvalue):
		cvimageflags = 1
		if compid in self.picameras:
			currentframe = cv2.imdecode(np.frombuffer(streamvalue, dtype='uint8'), cvimageflags)
			if currentframe is not None:
				self.picameras[compid].currentframe = currentframe

	def MAKE_MANAGER_EXECUTE(self, compid):
		if compid in self.functions:
			self.functions[compid](*self.args[compid])

class Proxy():

	def __init__(self, manager, compid, attrlist, methlist):
		object.__setattr__(self, '_manager',  manager)
		object.__setattr__(self, '_compid',   compid)
		object.__setattr__(self, '_attrlist', attrlist)
		for methodname in methlist:
			def method(self, *args, tcptimeout=10, methodname=methodname, **kwargs):
				return self._manager.execute(MAKE_COMPONENT_EXECUTE_OPCODE, compid, methodname, args, kwargs, timeout=tcptimeout)
			object.__setattr__(self, methodname, MethodType(method, self))
	
	def __getattr__(self, attrname, tcptimeout=10):
		if attrname in self._attrlist:
			return self._manager.execute(GET_COMPONENT_ATTRIBUTE_OPCODE, self._compid, attrname, timeout=tcptimeout)
		else:
			object.__getattr__(self, attrname)

	def __setattr__(self, attrname, attrvalue, tcptimeout=10):
		if attrname in self._attrlist:
			return self._manager.execute(SET_COMPONENT_ATTRIBUTE_OPCODE, self._compid, attrname, attrvalue, timeout=tcptimeout)
		else:
			object.__setattr__(self, attrname, attrvalue)

class SerialTalksProxy(Proxy):

	def __init__(self, manager, uuid):
		compid = manager.execute(CREATE_SERIALTALKS_COMPONENT_OPCODE, uuid)
		attrlist = ['port', 'is_connected']
		methlist = ['connect', 'disconnect', 'send', 'poll', 'flush', 'execute', 'getuuid', 'setuuid', 'getout', 'geterr']
		Proxy.__init__(self, manager, compid, attrlist, methlist)

class SwitchProxy(Proxy):

	def __init__(self, manager, switchpin):
		compid = manager.execute(CREATE_SWITCH_COMPONENT_OPCODE, switchpin)
		attrlist = ['state', 'PinInput']
		methlist = ['Close']
		Proxy.__init__(self, manager, compid, attrlist, methlist)

	def SetFunction(self, function, *args):
		self._manager.functions[self._compid] = function
		self._manager.args     [self._compid] = args

class LightButtonProxy(Proxy):

	def __init__(self, manager, switchpin, ledpin):
		compid = manager.execute(CREATE_LIGHTBUTTON_COMPONENT_OPCODE, switchpin, ledpin)
		attrlist = ['state', 'PinInput', 'PinLight']
		methlist = ['SetAutoSwitch', 'On', 'Off', 'Switch', 'Close']
		Proxy.__init__(self, manager, compid, attrlist, methlist)

	def SetFunction(self, function, *args):
		self._manager.functions[self._compid] = function
		self._manager.args     [self._compid] = args

try:
	import cv2
	import numpy as np
	
	class PiCameraProxy(Proxy):

		def __init__(self, manager, resolution, framerate):
			compid = manager.execute(CREATE_PICAMERA_COMPONENT_OPCODE, resolution, framerate)
			attrlist = ['resolution', 'framerate']
			methlist = ['start_capture', 'stop_capture']
			Proxy.__init__(self, manager, compid, attrlist, methlist)
			
			manager.picameras[compid] = self
			self.currentframe = np.array([]) # TODO: initialize with an empty frame	

		def get_current_frame(self):
			return self.currentframe.copy()
except ImportError:
	pass
	