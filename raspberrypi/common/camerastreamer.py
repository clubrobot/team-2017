#!/usr/bin/env python3
#-*- coding: utf-8 -*-

import os, sys
import io
from time      import time, sleep
from threading import RLock

# Raspberry Pi modules
try:
	from picamera import PiCamera
except ImportError as e:
	pass #sys.stderr.write('{}: {}\n'.format(type(e).__name__, e))

# Controller modules
try:
	import cv2
	import numpy as np
except ImportError as e:
	pass #sys.stderr.write('{}: {}\n'.format(type(e).__name__, e))

from common.tcptalks import TCPTalks

CAMERASTREAMER_DEFAULT_PORT = 25567

UPDATE_CAMERA_FRAME_OPCODE = 0x20
GET_CAMERA_PROPERTY_OPCODE = 0x21
SET_CAMERA_PROPERTY_OPCODE = 0x22


class CameraStreamer(TCPTalks):

	def __init__(self, port=CAMERASTREAMER_DEFAULT_PORT, password=None):
		TCPTalks.__init__(self, port=port, password=password)
		self.bind(GET_CAMERA_PROPERTY_OPCODE, self.get_camera_property)
		self.bind(SET_CAMERA_PROPERTY_OPCODE, self.set_camera_property)
		self.camera = PiCamera(resolution=(320, 240), framerate=24)
		sleep(2)

	def get_camera_property(self, propertyname):
		return getattr(self.camera, propertyname)
	
	def set_camera_property(self, propertyname, value):
		return setattr(self.camera, propertyname, value)
	
	def generate_streams_and_send_their_values(self):
		stream = io.BytesIO()
		while self.is_connected:

			# Give the hand to the PiCamera.capture_sequence method
			stream.seek(0)
			yield stream

			# Send the captured stream
			stream.truncate()
			self.send(UPDATE_CAMERA_FRAME_OPCODE, stream.getvalue())

	def capture(self):
		streams = self.generate_streams_and_send_their_values()
		self.camera.capture_sequence(streams, 'jpeg', use_video_port=True)


class Camera(TCPTalks):

	def __init__(self, ip, port=CAMERASTREAMER_DEFAULT_PORT, password=None):
		TCPTalks.__init__(self, ip, port=port, password=password)
		self.bind(UPDATE_CAMERA_FRAME_OPCODE, self.update_camera_frame)

		# Thread-safe frames management
		self.currentframe = np.array(list()) # TODO: initialize with an empty frame		
		self.frames_lock  = RLock()

	def update_camera_frame(self, streamvalue):
		cvimageflags = 1
		currentframe = cv2.imdecode(np.frombuffer(streamvalue, dtype='uint8'), cvimageflags)
		if currentframe is not None:
			self.frames_lock.acquire()
			self.currentframe = currentframe
			self.frames_lock.release()

	def get_current_frame(self):
		self.frames_lock.acquire()
		currentframe = self.currentframe.copy()
		self.frames_lock.release()
		return currentframe
'''
	def __getattr__(self, propertyname):
		return self.execute(GET_CAMERA_PROPERTY_OPCODE, propertyname)
	
	def __setattr__(self, propertyname, value):
		return self.execute(SET_CAMERA_PROPERTY_OPCODE, propertyname, value)
'''