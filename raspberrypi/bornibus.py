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
import re


class Bornibus(Behavior):

	def __init__(self, *args, **kwargs):
		Behavior.__init__(self, *args, **kwargs)
		self.setup_gripper_mandatory = False
		self.store_module_mandatory = False
		self.elevator_stucked = False

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
#			self.sensors     = Sensors(self)
		except:
			self.disconnect()
			raise

	def load_roadmap(self, filename):
		self.geogebra = GeoGebra(filename)
		self.roadmap = RoadMap.load(self.geogebra)

	def make_decision(self):
		import random
		actions = [
			(self.take_playfield_module_procedure, ('module_{01, action, a}',), {}, self.geogebra.get('module_{01, action, a}') + (self.geogebra.get('module_{01, angle, a}'),)),
			(self.take_playfield_module_procedure, ('module_{03, action, a}',), {}, self.geogebra.get('module_{03, action, a}') + (self.geogebra.get('module_{03, angle, a}'),)),
			(self.take_rocket_module_procedure, ('module_{06, action, a}',), {}, self.geogebra.get('module_{06, action, a}') + (math.pi,)),
			(self.drop_module_procedure, (), {}, self.geogebra.get('deposit_{00, action, a}') + (math.pi,)),
			(self.drop_module_procedure, (), {}, self.geogebra.get('deposit_{01, action, a}') + (math.pi,)),
			(self.drop_module_procedure, (), {}, self.geogebra.get('deposit_{02, action, a}') + (math.pi,)),
			(self.drop_module_procedure, (), {}, self.geogebra.get('deposit_{03, action, a}') + (math.pi,)),
			(self.drop_module_procedure, (), {}, self.geogebra.get('deposit_{04, action, a}') + (-math.pi/4,)),
			(self.drop_module_procedure, (), {}, self.geogebra.get('deposit_{04, action, b}') + (3*math.pi/4,)),
			(self.drop_module_procedure, (), {}, self.geogebra.get('deposit_{05, action, a}') + (-math.pi/4,)),
			(self.drop_module_procedure, (), {}, self.geogebra.get('deposit_{05, action, b}') + (3*math.pi/4,)),
			(self.drop_module_procedure, (), {}, self.geogebra.get('deposit_{06, action, b}') + (3*math.pi/4,))
		]
		action = random.choice(actions)
		if action[0] == self.take_playfield_module_procedure or action[0] == self.take_rocket_module_procedure:
			self.setup_gripper_mandatory = True
		return action
		return (None, (), {}, None)

	def goto_procedure(self, destination):
		# Pathfinding
		x_in, y_in, theta_in = self.wheeledbase.get_position()
		x_sp, y_sp, theta_sp = destination
		path = self.roadmap.get_shortest_path((x_in, y_in), (x_sp, y_sp))

		# Pure Pursuit configuration
		if math.cos(math.atan2(path[1][1] - path[0][1], path[1][0] - path[0][0]) - theta_in) >= 0:
			direction = 1
		else:
			direction = -1
		lookahead = 200
		linvelmax = 400
		angvelmax = 2

		# Trajectory
		self.wheeledbase.purepursuit(path, direction={1:'forward', -1:'backward'}[direction], lookahead=lookahead, linvelmax=linvelmax, angvelmax=angvelmax)

		# Do mandatory procedures
		def mandatory_procedures():		
			if self.store_module_mandatory:
				self.store_module_procedure()
			if self.setup_gripper_mandatory:
				self.setup_gripper_procedure()
		parallel_actions = self.perform(mandatory_procedures)

		# Wait until destination is reached
		isarrived = False
		blocked = False
		while not isarrived:

			# Manage sensors
#			distance = self.sensors.get_mesure()[(direction + 1) // 2]
#			if distance < 200:
#				return False
			
			# Get trajectory planner situation
			try:
				isarrived = self.wheeledbase.isarrived()
			except RuntimeError:
				blocked = True
			
			# Handle spin urgency
			if blocked:
				self.wheeledbase.set_velocities(-direction * 100, 0)
				time.sleep(1)
				self.wheeledbase.purepursuit(path, direction={1:'forward', -1:'backward'}[direction])
				blocked = False
			
			# Delay
			time.sleep(0.1)
		
		# Wait for mandatory actions
		self.get(parallel_actions)

		# Turn on the spot
		if theta_sp is not None:
			self.wheeledbase.turnonthespot(theta_sp)		
			try:
				self.wheeledbase.wait()
			except RuntimeError:
				return False
		
		# Everything is fine
		return True

	def stop_procedure(self):
		self.wheeledbase.stop()
		self.gripper.set_position(-1)
		self.elevator.set_velocity(0)
		self.dispenser.set_position(-1, 1)

	def take_playfield_module_procedure(self, label):
		# Go to the taking point
		module, action = re.match('module_{(\d+), action, ([a-c])}', label).groups()
		self.wheeledbase.goto(*self.geogebra.get('module_{{{}, action, {}, {}}}'.format(module, action, 1)))

		# Do an adjustment procedure
		self.wheeledbase.set_velocities(30,  0.0); time.sleep(0.6)
		self.wheeledbase.set_velocities(20, -1.0); time.sleep(0.4)
		self.wheeledbase.set_velocities(0, 0)
		
		# Hold the module
		self.gripper.close()
		time.sleep(0.4)

		# Store the module during the next route
		self.store_module_mandatory = True
	
	def take_rocket_module_procedure(self, label):
		# Go to the taking point
		module, action = re.match('module_{(\d+), action, ([a-c])}', label).groups()
		self.wheeledbase.goto(*self.geogebra.get('module_{{{}, action, {}, {}}}'.format(module, action, 1)))
		
		# Get to the bottom of the rocket
		self.wheeledbase.set_velocities(45, -0.3); time.sleep(0.2)
		self.wheeledbase.set_velocities(0, 0)
		
		# Hold the module
		self.gripper.close()
		time.sleep(0.4)

		# Go backward
		self.wheeledbase.goto(*self.geogebra.get(label))

		# Do an adjustment procedure
		self.gripper.open_low();
		self.wheeledbase.set_velocities(60, 0); time.sleep(0.4);
		self.wheeledbase.set_velocities(0, 0);
		
		# Hold the module
		self.gripper.close();
		time.sleep(0.4)

		# Store the module during the next route
		self.store_module_mandatory = True

	def store_module_procedure(self, delay=0):
		try:
			self.elevator.go_up()
			time.sleep(delay)
			self.gripper.open_up()
			time.sleep(1)
		except RuntimeError:
			self.elevator_stucked = True
		self.store_module_mandatory = False

	def setup_gripper_procedure(self):
		self.gripper.close()
		time.sleep(0.4)
		try:
			self.elevator.go_down()
			self.gripper.open_low()
		except RuntimeError:
			self.elevator_stucked = True
		self.setup_gripper_mandatory = False

	def drop_module_procedure(self):
		self.dispenser.open()
		time.sleep(1.2)
		self.dispenser.close()
		time.sleep(0.7)
