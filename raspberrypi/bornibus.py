#!/usr/bin/env python3
#-*- coding: utf-8 -*-

from behaviour import Behavior, Gesture

from wheeledbase      import wheeledbase
from modulescollector import ModulesGripper, ModulesElevator, ModulesDispenser
from display          import LEDMatrix, SevenSegments
from sensors          import Sensors


class Bornibus(Behavior):

	def __init__(self, *args, **kwargs):
		Behavior.__init__(self, *args, **kwargs):
		# TODO: init actions, etc.

	def connect(self):
		try:
			self.connect()
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

#TODO: create Bornibus gestures and actions
