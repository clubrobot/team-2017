#!/usr/bin/env python3
#-*- coding: utf-8 -*-

from behavior import Behavior

from wheeledbase      import WheeledBase
from modulescollector import ModulesGripper, ModulesElevator, ModulesDispenser, Mustaches
from display          import LEDMatrix, SevenSegments
from sensors          import Sensors
from components       import LightButtonProxy, SwitchProxy

from brother import Brother

from geogebra import GeoGebra
from roadmap import RoadMap, intersect

import time
import math


class BornibusBrother(Brother):

	def get_brother_position(self):
		return self.brother.wheeledbase.get_position()
	
	def get_brother_shape(self):
		x, y, theta = self.brother.wheeledbase.get_position()
		W = self.brother.geogebra.get('Bornibus_{width}')
		L = self.brother.geogebra.get('Bornibus_{length}')
		shape = []
		for dx, dy in (L / 2, W / 2), (-L / 2, W / 2), (-L / 2, -W / 2), (L / 2, -W / 2):
			xi = x + dx * math.cos(theta) - dy * math.sin(theta)
			yi = y + dx * math.sin(theta) + dy * math.cos(theta)
			shape.append((x, y))
		return shape


class Bornibus(Behavior):

	def __init__(self, *args, **kwargs):
		Behavior.__init__(self, *args, **kwargs)
		self.setup_gripper_mandatory = False
		self.store_module_mandatory = False
		self.elevator_stucked = False

		self.automatestep = 0

	def connect(self, *args, **kwargs):
		try:
			Behavior.connect(self, *args, **kwargs)
			self.wheeledbase    = WheeledBase(self)
			self.gripper        = ModulesGripper(self)
			self.elevator       = ModulesElevator(self)
			self.dispenser      = ModulesDispenser(self)
			self.mustaches      = Mustaches(self)
			self.left_eye       = LEDMatrix(self, 1)
			self.right_eye      = LEDMatrix(self, 2)
			self.display        = SevenSegments(self)
			self.frontsensors   = Sensors(self, 'frontsensors')
			self.backsensors    = Sensors(self, 'backsensors')
			self.redbutton      = LightButtonProxy(self, 15, 16)
			self.bluebutton     = LightButtonProxy(self, 23, 24)
			self.yellowbutton   = LightButtonProxy(self, 35, 36)
			self.greenbutton    = LightButtonProxy(self, 21, 22)
			self.pullswitch     = SwitchProxy(self, 29)
		except:
			self.disconnect()
			raise
		
		self.gripper.high_open_angle = 145
		self.gripper.low_open_angle = 80
		self.gripper.close_angle = 5
		self.gripper.set_velocity(300)

		self.front_sensors_angles = (math.radians( +20), math.radians( -42))
		self.back_sensors_angles  = (math.radians(-140), math.radians(+163))

	def load_roadmap(self, filename):
		self.geogebra = GeoGebra(filename)
		self.roadmap = RoadMap.load(self.geogebra)

		module01 = TakePlayfieldModuleAction(self.geogebra, '01', 'a')
		module03 = TakePlayfieldModuleAction(self.geogebra, '03', 'a')
		module05 = HoldPlayfieldModuleAction(self.geogebra, '05', 'a')
		module06 = TakeRocketModuleAction(self.geogebra, '06', 'a')
		deposit00 = DropModuleAction(self.geogebra, '00', 'a')
		deposit01 = DropModuleAction(self.geogebra, '01', 'a')
		deposit02 = DropModuleAction(self.geogebra, '02', 'a')
		deposit03 = DropModuleAction(self.geogebra, '03', 'a')
		deposit04 = DropModuleAction(self.geogebra, '04', 'b')
		deposit05 = DropModuleAction(self.geogebra, '05', 'b')
		deposit06 = DropModuleAction(self.geogebra, '06', 'b')
		shift0406 = DropAndShiftModuleAction(self.geogebra, '04', 'b', '06', 'b')
		deposit23 = ReleaseModuleAction(self.geogebra, '23', 'a')
		odometry00b = RecalibrateOdometryAction(self.geogebra, '00', 'b')
		odometry00c = RecalibrateOdometryAction(self.geogebra, '00', 'c')
		odometry02a = RecalibrateOdometryAction(self.geogebra, '02', 'a')
		odometry02b = RecalibrateOdometryAction(self.geogebra, '02', 'b')
		
		self.automate = [
			module03,
			module01,
			odometry00b,
			deposit01,
			deposit00,
			module06,
			module06,
			module06,
			module06,
			odometry02b,
			deposit05,
			shift0406,
			deposit05,
			deposit04
		]

	def connect_to_brother(self, *args, **kwargs):
		self.brother = BornibusBrother(self, *args, **kwargs)
		self.brother.start()

	def make_decision(self):
		action = self.automate[self.automatestep]
		if isinstance(action, (TakePlayfieldModuleAction, TakeRocketModuleAction, HoldPlayfieldModuleAction)):
			self.setup_gripper_mandatory = True
		return action.procedure, (self,), {}, action.actionpoint + (action.orientation,)

	def goto_procedure(self, destination):
		wheeledbase = self.wheeledbase

		# Pathfinding
		path_not_found = False
		x_in, y_in, theta_in = wheeledbase.get_position()
		x_sp, y_sp, theta_sp = destination
		try:
			path = self.roadmap.get_shortest_path((x_in, y_in), (x_sp, y_sp))
			self.log('follow path: [{}]'.format(', '.join('({0[0]:.0f}, {0[1]:.0f})'.format(waypoint) for waypoint in path)))
		except RuntimeError:
			path_not_found = True

		# Reset detected obstacles
		self.log('reset edges')
		self.roadmap.reset_edges()

		# Return there is no path available		
		if path_not_found:
			self.log('no path found')
			time.sleep(1)
			return False

		# Pure Pursuit configuration
		if math.cos(math.atan2(path[1][1] - path[0][1], path[1][0] - path[0][0]) - theta_in) >= 0:
			direction = 1
		else:
			direction = -1
		wheeledbase.lookahead.set(150)
		wheeledbase.lookaheadbis.set(150)
		wheeledbase.max_linvel.set(500)
		wheeledbase.max_angvel.set(6.0)
		wheeledbase.linpos_threshold.set(3)
		wheeledbase.angpos_threshold.set(0.1)

		# Trajectory
		wheeledbase.purepursuit(path, direction={1:'forward', -1:'backward'}[direction])

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

			# Get current position
			x, y, theta = wheeledbase.get_position()

			# Manage sensors
			found_obstacle = False
				
			# Abort the current action if an obstacle was found
			if found_obstacle:
				wheeledbase.stop()
				self.interrupt(parallel_actions)
				return False

			# Get trajectory planner situation
			try:
				isarrived = wheeledbase.isarrived()
			except RuntimeError:
				self.log('blocked while following path')
				blocked = True
			
			# Handle spin urgency
			if blocked:
				self.log('go backward a little')
				wheeledbase.set_velocities(-direction * 100, 0)
				time.sleep(1)
				self.log('resume path')
				wheeledbase.purepursuit(path, direction={1:'forward', -1:'backward'}[direction])
				blocked = False
			
			# Delay
			time.sleep(0.1)
		self.log('path ended at: ({0[0]:.0f}, {0[1]:.0f}, {0[2]:.2f})'.format(wheeledbase.get_position()))

		# Turn on the spot
		if theta_sp is not None:
			self.log('turn on the spot: {:.2f}'.format(theta_sp))
			wheeledbase.turnonthespot(theta_sp)		
			try:
				wheeledbase.wait()
			except RuntimeError:
				self.log('blocked while turning on the spot')
				return False
		self.log('rotation ended at: ({0[0]:.0f}, {0[1]:.0f}, {0[2]:.2f})'.format(wheeledbase.get_position()))
		
		# Wait for mandatory actions
		self.get(parallel_actions)
		self.automatestep += 1

		# Everything is fine
		return True

	def stop_procedure(self):
		self.wheeledbase.stop()
		self.gripper.set_position(-1)
		self.elevator.set_velocity(0)
		self.dispenser.set_position(-1, 1)

	def store_module_procedure(self, delay=0):
		self.log('store module')
		try:
			self.elevator.go_up()
			time.sleep(delay)
			self.gripper.open_up()
			time.sleep(1)
		except RuntimeError:
			self.log('elevator stucked')
			self.elevator_stucked = True
		self.store_module_mandatory = False

	def setup_gripper_procedure(self):
		self.log('setup gripper')
		self.gripper.close()
		time.sleep(0.4)
		try:
			self.elevator.go_down()
			self.gripper.open_low()
		except RuntimeError:
			self.log('elevator stucked')
			self.elevator_stucked = True
		self.setup_gripper_mandatory = False


class TakePlayfieldModuleAction:
	def __init__(self, geogebra, major, minor):
		self.actionpoint = geogebra.get('module_{{{}, action, {}}}'.format(major, minor))
		self.takingpoint = geogebra.get('module_{{{}, action, {}, 1}}'.format(major, minor))
		self.orientation = math.atan2(self.takingpoint[1] - self.actionpoint[1], self.takingpoint[0] - self.actionpoint[0])
		
	def procedure(self, bornibus):
		bornibus.log('take playfield module')
		wheeledbase = bornibus.wheeledbase
		gripper     = bornibus.gripper

		# Go to the taking point
		wheeledbase.lookahead.set(70)
		wheeledbase.goto(*self.takingpoint)

		# Do an adjustment procedure
		wheeledbase.set_velocities(30,  0.0); time.sleep(0.6)
		wheeledbase.set_velocities(20, -1.0); time.sleep(0.4)
		wheeledbase.set_velocities(0, 0)
		
		# Hold the module
		gripper.close()
		time.sleep(0.4)
		
		# Store the module during the next route
		bornibus.store_module_mandatory = True


class TakeRocketModuleAction:
	def __init__(self, geogebra, major, minor):
		self.actionpoint = geogebra.get('module_{{{}, action, {}}}'.format(major, minor))
		self.takingpoint = geogebra.get('module_{{{}, action, {}, 1}}'.format(major, minor))
		self.stabilizationpoint = geogebra.get('module_{{{}, action, {}, 2}}'.format(major, minor))
		self.orientation = math.atan2(self.takingpoint[1] - self.actionpoint[1], self.takingpoint[0] - self.actionpoint[0])
		self.remaining = 4
				
	def procedure(self, bornibus):
		bornibus.log('take rocket module')
		wheeledbase = bornibus.wheeledbase
		gripper     = bornibus.gripper

		# Go to the taking point
		try: wheeledbase.goto(*self.takingpoint)
		except RuntimeError: pass
		
		# Get to the bottom of the rocket
		wheeledbase.set_velocities(45, -0.3); time.sleep(0.2)
		wheeledbase.set_velocities(0, 0)
		
		# Hold the module
		gripper.close()
		time.sleep(0.4)
		self.remaining -= 1
			
		# Stabilize the last module
		if self.remaining == 1:
			wheeledbase.goto(*self.stabilizationpoint)

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
		
	def procedure(self, bornibus):
		bornibus.log('drop module')
		dispenser = bornibus.dispenser

		# Drop module
		dispenser.open()
		time.sleep(1.2)
		
		# Close dispenser
		dispenser.close()
		time.sleep(0.7)


class DropAndShiftModuleAction:
	def __init__(self, geogebra, major1, minor1, major2, minor2):
		deposit          = geogebra.get('deposit_{{{}}}'.format(major1))
		self.actionpoint = geogebra.get('deposit_{{{}, action, {}}}'.format(major1, minor1))
		self.shiftpoint  = geogebra.get('deposit_{{{}, action, {}}}'.format(major2, minor2))
		self.orientation = math.pi / 2 + math.atan2(deposit[1] - self.actionpoint[1], deposit[0] - self.actionpoint[0])
		
	def procedure(self, bornibus):
		bornibus.log('drop and shift module')
		wheeledbase = bornibus.wheeledbase
		dispenser   = bornibus.dispenser

		# Drop module
		dispenser.open()
		time.sleep(1.2)
		
		# Shift module
		wheeledbase.goto(*self.shiftpoint)

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
		bornibus.log('recalibrate odometry')
		wheeledbase = bornibus.wheeledbase
		mustaches   = bornibus.mustaches

		# Run at the wall
		wheeledbase.set_velocities(200, 0)
		try: wheeledbase.wait()
		except RuntimeError: wheeledbase.set_openloop_velocities(500, 500)
		time.sleep(0.5)

		# Check if there is no obstacle on the route
		if not mustaches.get_left_mustache() or not mustaches.get_right_mustache():
			bornibus.log('found obstacle')
		
			# Go backward a little
			wheeledbase.goto(*self.sweepobstacle)

			# Quickly turn on the spot to sweep the obstacle away
			wheeledbase.set_velocities(0, 10)
			time.sleep(1)

			# Run at the wall again
			wheeledbase.turnonthespot(self.orientation)
			wheeledbase.wait()
			wheeledbase.set_velocities(200, 0)
			try: wheeledbase.wait()
			except RuntimeError: wheeledbase.set_openloop_velocities(500, 500)
			time.sleep(0.5)
		
		# Check if there is still no obstacle on the route
		if mustaches.get_left_mustache() and mustaches.get_right_mustache():
			bornibus.log('found no obstacle')

			# Do an odometry recalibration
			xref, yref = self.calibration
			thetaref = self.orientation
	#		thetaref = wheeledbase.get_position()[2]
			xthought, ythought = wheeledbase.get_position()[:2]
			offset = math.hypot(xref - xthought, yref - ythought) * math.cos(thetaref - math.atan2(yref - ythought, xref - xthought))
			xthought += offset * math.cos(thetaref)
			ythought += offset * math.sin(thetaref)
			wheeledbase.set_position(xthought, ythought, thetaref)
		else:
			bornibus.log('found another obstacle')
		
		# Stop running
		wheeledbase.stop()


class HoldPlayfieldModuleAction:
	def __init__(self, geogebra, major, minor):
		self.actionpoint = geogebra.get('module_{{{}, action, {}}}'.format(major, minor))
		self.takingpoint = geogebra.get('module_{{{}, action, {}, 1}}'.format(major, minor))
		self.orientation = math.atan2(self.takingpoint[1] - self.actionpoint[1], self.takingpoint[0] - self.actionpoint[0])
		
	def procedure(self, bornibus):
		bornibus.log('hold playfield module')
		wheeledbase = bornibus.wheeledbase
		gripper     = bornibus.gripper

		# Go to the taking point
		wheeledbase.goto(*self.takingpoint)
		
		# Hold the module
		gripper.close()
		time.sleep(0.4)
		

class ReleaseModuleAction:
	def __init__(self, geogebra, major, minor):
		deposit          = geogebra.get('deposit_{{{}}}'.format(major))
		self.actionpoint = geogebra.get('deposit_{{{}, action, {}}}'.format(major, minor))
		self.orientation = math.atan2(deposit[1] - self.actionpoint[1], deposit[0] - self.actionpoint[0]) - math.atan2(geogebra.get('BornibusGripper_{xoffset}'), geogebra.get('BornibusGripper_{yoffset}'))
		
	def procedure(self, bornibus):
		bornibus.log('release module')
		wheeledbase = bornibus.wheeledbase
		gripper     = bornibus.gripper

		# Release the module
		gripper.open_low()
		time.sleep(0.4)
		