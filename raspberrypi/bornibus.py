#!/usr/bin/env python3
#-*- coding: utf-8 -*-

from behavior import Behavior

from wheeledbase      import WheeledBase
from modulescollector import ModulesGripper, ModulesElevator, ModulesDispenser
from display          import LEDMatrix, SevenSegments
from sensors          import Sensors

from geogebra import GeoGebra
from roadmap import RoadMap

import time
import math


class Bornibus(Behavior):

	def __init__(self, *args, **kwargs):
		Behavior.__init__(self, *args, **kwargs)

	def connect(self):
		try:
			Behavior.connect(self)
			self.wheeledbase = WheeledBase(self)
			self.gripper     = ModulesGripper(self)
			self.elevator    = ModulesElevator(self)
			self.dispenser   = ModulesDispenser(self)
			self.left_eye    = LEDMatrix(self, 1)
			self.right_eye   = LEDMatrix(self, 2)
			self.display     = SevenSegments(self)
			self.sensors     = Sensors(self)
		except:
			self.disconnect()
			raise

	def load_roadmap(self, filename):
		self.geogebra = GeoGebra(filename)
		self.roadmap = RoadMap.load(self.geogebra)

	def make_decision(self):
		import random
		return (self.simple_action, (), {}, self.geogebra.get(random.choice(['A','B','C','D','E'])) + (None,))
		return (None, (), {}, None)

	def goto_procedure(self, destination):
		# Pathfinding
		x0, y0, theta0 = self.wheeledbase.get_position()
		x1, y1, theta1 = destination
		path = self.roadmap.get_shortest_path((x0, y0), (x1, y1))

		# Pure Pursuit configuration
		if math.cos(math.atan2(path[1][1] - path[0][1], path[1][0] - path[0][0]) - theta0) >= 0:
			direction = 'forward'
		else:
			direction = 'backward'
		lookahead = 150
		linvelmax = 400
		angvelmax = 3

		# Trajectory
		self.wheeledbase.purepursuit(path, direction=direction, lookahead=lookahead, linvelmax=linvelmax, angvelmax=angvelmax)

		# Wait until destination is reached
		while True:
			distance = self.sensors.get_mesure()[{'forward':1, 'backward':0}[direction]]
			if distance < 100:
				return False

			try:
				if self.wheeledbase.isarrived():
					break

			except RuntimeError: # Handle spin urgency
				linvel = -100 * {'forward':1, 'backward':-1}[direction]
				self.wheeledbase.set_velocities(linvel, 0)
				time.sleep(1)
				self.wheeledbase.purepursuit(path, direction=direction)

			time.sleep(0.1)
		
		# Turn on the spot
		if theta1 is not None:
			self.wheeledbase.turnonthespot(theta1)
			try:
				self.wheeledbase.wait()
			except RuntimeError:
				return False
		
		# Everything is fine
		return True

	def simple_action(self):
		self.gripper.close()
		time.sleep(0.5)
		self.gripper.open_low()
		time.sleep(0.5)
