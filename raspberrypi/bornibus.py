#!/usr/bin/env python3
#-*- coding: utf-8 -*-

from behavior import Behavior

from wheeledbase      import WheeledBase
from modulescollector import ModulesGripper, ModulesElevator, ModulesDispenser, Mustaches
from display          import LEDMatrix, SevenSegments
from sensors          import Sensors

from geogebra import GeoGebra
from roadmap import RoadMap

import time
import math


class Bornibus(Behavior):

	def __init__(self, *args, **kwargs):
		Behavior.__init__(self, *args, **kwargs)
		self.setup_gripper_mandatory = False
		self.store_module_mandatory = False
		self.calibration_required = False
		self.elevator_stucked = False

	def connect(self):
		try:
			Behavior.connect(self)
			self.wheeledbase = WheeledBase(self)
			self.gripper     = ModulesGripper(self)
			self.elevator    = ModulesElevator(self)
			self.dispenser   = ModulesDispenser(self)
			self.mustaches   = Mustaches(self)
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
#			TakePlayfieldModuleAction(self.geogebra, '01', 'a'),
#			TakePlayfieldModuleAction(self.geogebra, '03', 'a')
#			TakeRocketModuleAction(self.geogebra, '06', 'a'),
#			DropModuleAction(self.geogebra, '00', 'a'),
#			DropModuleAction(self.geogebra, '01', 'a'),
#			DropModuleAction(self.geogebra, '02', 'a'),
#			DropModuleAction(self.geogebra, '03', 'a'),
#			DropModuleAction(self.geogebra, '04', 'a'),
#			DropModuleAction(self.geogebra, '04', 'b'),
#			DropModuleAction(self.geogebra, '05', 'a'),
#			DropModuleAction(self.geogebra, '05', 'b'),
#			DropModuleAction(self.geogebra, '06', 'b'),
			RecalibrateOdometryAction(self.geogebra, '00', 'a'),
			RecalibrateOdometryAction(self.geogebra, '00', 'b'),
			RecalibrateOdometryAction(self.geogebra, '00', 'c'),
			RecalibrateOdometryAction(self.geogebra, '00', 'd'),
			RecalibrateOdometryAction(self.geogebra, '01', 'a'),
			RecalibrateOdometryAction(self.geogebra, '02', 'a'),
			RecalibrateOdometryAction(self.geogebra, '02', 'b')
		]
		action = random.choice(actions)
		if isinstance(action, (TakePlayfieldModuleAction, TakeRocketModuleAction)):
			self.setup_gripper_mandatory = True
		return action.procedure, (self,), {}, action.actionpoint + (action.orientation,)

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
		self.wheeledbase.lookahead.set(200)
		self.wheeledbase.max_linvel.set(400)
		self.wheeledbase.max_angvel.set(3.0)
		self.wheeledbase.linpos_threshold.set(3)
		self.wheeledbase.angpos_threshold.set(0.1)

		# Trajectory
		self.wheeledbase.purepursuit(path, direction={1:'forward', -1:'backward'}[direction])

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

		# Turn on the spot
		if theta_sp is not None:
			self.wheeledbase.turnonthespot(theta_sp)		
			try:
				self.wheeledbase.wait()
			except RuntimeError:
				return False
		
		# Wait for mandatory actions
		self.get(parallel_actions)

		# Everything is fine
		return True

	def stop_procedure(self):
		self.wheeledbase.stop()
		self.gripper.set_position(-1)
		self.elevator.set_velocity(0)
		self.dispenser.set_position(-1, 1)

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


class TakePlayfieldModuleAction:
	def __init__(self, geogebra, major, minor):
		self.actionpoint = geogebra.get('module_{{{}, action, {}}}'.format(major, minor))
		self.takingpoint = geogebra.get('module_{{{}, action, {}, 1}}'.format(major, minor))
		self.orientation = math.atan2(self.takingpoint[1] - self.actionpoint[1], self.takingpoint[0] - self.actionpoint[0])
		self.isdone = False

	def procedure(self, bornibus):
		wheeledbase = bornibus.wheeledbase
		gripper     = bornibus.gripper

		# Go to the taking point
		wheeledbase.goto(*self.takingpoint)

		# Do an adjustment procedure
		wheeledbase.set_velocities(30,  0.0); time.sleep(0.6)
		wheeledbase.set_velocities(20, -1.0); time.sleep(0.4)
		wheeledbase.set_velocities(0, 0)
		
		# Hold the module
		gripper.close()
		time.sleep(0.4)
		self.isdone = True

		# Store the module during the next route
		bornibus.store_module_mandatory = True


class TakeRocketModuleAction:
	def __init__(self, geogebra, major, minor):
		self.actionpoint = geogebra.get('module_{{{}, action, {}}}'.format(major, minor))
		self.takingpoint = geogebra.get('module_{{{}, action, {}, 1}}'.format(major, minor))
		self.orientation = math.atan2(self.takingpoint[1] - self.actionpoint[1], self.takingpoint[0] - self.actionpoint[0])
		self.remaining = 4
		self.isdone = False
		
	def procedure(self, bornibus):
		wheeledbase = bornibus.wheeledbase
		gripper     = bornibus.gripper

		# Go to the taking point
		wheeledbase.goto(*self.takingpoint)
		
		# Get to the bottom of the rocket
		wheeledbase.set_velocities(45, -0.3); time.sleep(0.2)
		wheeledbase.set_velocities(0, 0)
		
		# Hold the module
		gripper.close()
		time.sleep(0.4)
		self.remaining -= 1
		if self.remaining == 0:
			self.isdone = True

		# Go backward
		wheeledbase.goto(*self.actionpoint)

		# Do an adjustment procedure
		gripper.open_low();
		wheeledbase.set_velocities(60, 0); time.sleep(0.4);
		wheeledbase.set_velocities(0, 0);
		
		# Hold the module
		gripper.close();
		time.sleep(0.4)

		# Store the module during the next route
		bornibus.store_module_mandatory = True


class DropModuleAction:
	def __init__(self, geogebra, major, minor):
		deposit          = geogebra.get('deposit_{{{}}}'.format(major))
		self.actionpoint = geogebra.get('deposit_{{{}, action, {}}}'.format(major, minor))
		self.orientation = math.pi / 2 + math.atan2(deposit[1] - self.actionpoint[1], deposit[0] - self.actionpoint[0])
		self.isdone = False

	def procedure(self, bornibus):
		dispenser = bornibus.dispenser

		# Drop module
		dispenser.open()
		time.sleep(1.2)
		self.isdone = True

		# Close dispenser
		dispenser.close()
		time.sleep(0.7)


class RecalibrateOdometryAction:
	def __init__(self, geogebra, major, minor):
		self.actionpoint   = geogebra.get('odometry_{{{}, action, {}}}'.format(major, minor))
		self.calibration   = geogebra.get('odometry_{{{}, action, {}, 1}}'.format(major, minor))
		self.sweepobstacle = geogebra.get('odometry_{{{}, action, {}, 2}}'.format(major, minor))
		self.orientation = math.atan2(self.calibration[1] - self.actionpoint[1], self.calibration[0] - self.actionpoint[0])
	
	def procedure(self, bornibus):
		wheeledbase = bornibus.wheeledbase
		mustaches   = bornibus.mustaches

		# Run at the wall
		wheeledbase.set_velocities(200, 0)
		try: wheeledbase.wait()
		except RuntimeError: wheeledbase.set_openloop_velocities(500, 500)
		time.sleep(0.1)

		# Check if there is no obstacle on the route
		if not mustaches.get_left_mustache() or not mustaches.get_right_mustache():
			
			# Go backward a little
			wheeledbase.goto(*self.sweepobstacle)

			# Quickly turn on the spot to sweep the obstacle away
			wheeledbase.set_velocities(0, 6)
			time.sleep(1)

			# Run at the wall again
			wheeledbase.turnonthespot(self.orientation)
			wheeledbase.wait()
			wheeledbase.set_velocities(200, 0)
			try: wheeledbase.wait()
			except RuntimeError: wheeledbase.set_openloop_velocities(500, 500)
			time.sleep(0.1)
		
		# Do an odometry recalibration
		xref, yref = self.calibration
		thetaref = self.orientation
#		thetaref = wheeledbase.get_position()[3]
		xthought, ythought = wheeledbase.get_position()[:2]
		offset = math.hypot(xref - xthought, yref - ythought) * math.cos(thetaref - math.atan2(yref - ythought, xref - xthought))
		xthought += offset * math.cos(thetaref)
		ythought += offset * math.sin(thetaref)
		wheeledbase.set_position(xthought, ythought, thetaref)
		
		# Stop running
		wheeledbase.stop()
