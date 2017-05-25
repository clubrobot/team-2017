#!/usr/bin/env python3
#-*- coding: utf-8 -*-

from threading import Thread
from bluetalks import BlueTalks

import time
import math

from roadmap import intersect


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
	
	def get_edges(self, halo=0):
		try:
			edges = []
			print('shape', self.shape)
			for i in range(len(self.shape)):
				edges.append((self.shape[i], self.shape[(i+1) % len(self.shape)]))
			print('edges', edges)
			return edges			
		except AttributeError:
			return []

	def get_distance(self, x, y):
		distance = math.inf
		for edge in self.get_edges(halo=0):
			dx = x - edge[0][0]
			dy = y - edge[0][1]
			edgedx = edge[1][0] - edge[0][0]
			edgedy = edge[1][1] - edge[0][1]
			edgelength = math.hypot(edgedx, edgedy)
			t = (edgedx * dx + edgedy * dy) / edgelength**2
			if t > 1:
				h = math.hypot(x - edge[1][0], y - edge[1][1])
			elif t < 0:
				h = math.hypot(x - edge[0][0], y - edge[0][1])
			else:
				h = abs(edgedx * dy - edgedy * dx) / edgelength;
			if distance > h:
				distance = h
		return distance

	def is_on_path(self, path):
		edges = self.get_edges()
		for subpath in zip(path[:-1], path[1:]):
			for edge in edges:
				if intersect(edge, subpath):
					return True
		return False
