#!/usr/bin/env python3
#-*- coding: utf-8 -*-

from threading import Thread
from bluetalks import BlueTalks

import time
import math


class Brother(Thread):

	def __init__(self, brother, macaddress=None, channel=7):
		Thread.__init__(self)
		self.daemon = True
		self.brother = brother
		
		# Create a BlueTalks instance
		self.talks = BlueTalks(macaddress, channel)
		self.talks.bind('update', self.update_self)

	def update_self(self, position, shape):
		self.position = position
		self.shape = shape

	def get_brother_position(self):
		raise RuntimeError("the 'get_brother_position' method must be overriden")

	def get_brother_shape(self):
		raise RuntimeError("the 'get_brother_shape' method must be overriden")

	def update_brother(self):
		position = self.get_brother_position()
		shape = self.get_brother_shape()
		self.talks.execute('update', position, shape)

	def run(self):
		try:
			# Wait for a Bluetooth connection
			self.brother.log('wait for a Bluetooth connection')
			self.talks.connect(None)
			self.brother.log('found a Bluetooth connection')

			while True:
				# Check if the connection is still alive
				if not self.talks.listener.is_alive():
					raise RuntimeError('Bluetooth listener is dead')

				# Compute self position and shape
				self.update_brother()

				# Repeat
				time.sleep(0.5)
		finally:
			# Unset the variables in case of a Bluetooth crash
			del self.position
			del self.shape

