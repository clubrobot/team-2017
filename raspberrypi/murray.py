#!/usr/bin/env python3
#-*- coding: utf-8 -*-

from behavior import Behavior

from wheeledbase       import WheeledBase
from mineralscollector import AX12, Hammer, Roller, LaunchPad
from display           import LEDMatrix, SevenSegments
from sensors           import Sensors
from components        import LightButtonProxy, SwitchProxy

from brother import Brother

from geogebra import GeoGebra
from roadmap import RoadMap, intersect

import time
import math


class MurrayBrother(Brother):

	def get_brother_position(self):
		return self.brother.wheeledbase.get_position()
	
	def get_brother_shape(self):
		x, y, theta = self.brother.wheeledbase.get_position()
		W = self.brother.geogebra.get('Murray_{width}')
		F = self.brother.geogebra.get('Murray_{front}')
		B = self.brother.geogebra.get('Murray_{back}')
		shape = []
		for dx, dy in (F / 2, W / 2), (-B / 2, W / 2), (-B / 2, -W / 2), (F / 2, -W / 2):
			xi = x + dx * math.cos(theta) - dy * math.sin(theta)
			yi = y + dx * math.sin(theta) + dy * math.cos(theta)
			shape.append((x, y))
		return shape


class Murray(Behavior):

	def __init__(self, *args, **kwargs):
		Behavior.__init__(self, *args, **kwargs)
		self.storage_mandatory = False
		self.stored_minerals = 0
		self.minerals_05_storage_position = 280
		self.minerals_10_storage_position = 230
		self.minerals_15_storage_position = 170
		self.firing_cadency = 0.2 # Seconds per mineral

		self.automatestep = 0

	def connect(self, *args, **kwargs):
		try:
			Behavior.connect(self, *args, **kwargs)
			self.wheeledbase  = WheeledBase(self)
			self.roller       = Roller(self)
			self.rollerarm    = AX12(self)
			self.ballzooka    = Hammer(self)
#			self.left_eye     = LEDMatrix(self, 1)
#			self.right_eye    = LEDMatrix(self, 2)
#			self.display      = SevenSegments(self)
			self.redbutton    = LightButtonProxy(self, 15, 16)
			self.bluebutton   = LightButtonProxy(self, 23, 24)
			self.yellowbutton = LightButtonProxy(self, 35, 36)
			self.greenbutton  = LightButtonProxy(self, 21, 22)
			self.pullswitch   = SwitchProxy(self, 29)
			self.launchpad    = LaunchPad(self)
		except:
			self.disconnect()
			raise
		try:
			frontsensors = Sensors(self, 'frontsensors')
			backsensors  = Sensors(self, 'backsensors')
			self.frontsensors = frontsensors
			self.backsensors = backsensors
		except:
			pass

	def load_roadmap(self, filename):
		self.geogebra = GeoGebra(filename)
		self.roadmap = RoadMap.load(self.geogebra)

		# Yellow side
		crater0a = GatherBigCraterAction(self.geogebra, '0', 'a')
		crater0b = GatherBigCraterAction(self.geogebra, '0', 'b')
		crater1 = GatherSmallCraterAction(self.geogebra, '1', 'a')
		crater2 = GatherSmallCraterAction(self.geogebra, '2', 'a')
		hold0 = FireMineralsAction(self.geogebra, '0', 'a')
		module04 = StrikeModuleAction(self.geogebra, '04', 'a')

		# Blue side
		crater3 = GatherSmallCraterAction(self.geogebra, '3', 'a')
		crater4 = GatherSmallCraterAction(self.geogebra, '4', 'a')
		crater5a = GatherBigCraterAction(self.geogebra, '5', 'a')
		crater5b = GatherBigCraterAction(self.geogebra, '5', 'b')
		hold1 = FireMineralsAction(self.geogebra, '1', 'a')
		module08 = StrikeModuleAction(self.geogebra, '08', 'a')

		self.automate = [
			[
				crater1,
				crater0a,
				hold0,
				crater2,
				crater0b,
				hold0,
				module04
			],
			[
				crater4,
				crater5a,
				hold1,
				crater3,
				crater5b,
				hold1,
				module08
			]
		]

	def connect_to_brother(self, *args, **kwargs):
		self.brother = MurrayBrother(self, *args, **kwargs)
		self.brother.start()

	def make_decision(self):
		action = self.automate[self.side][self.automatestep]
		if hasattr(action, 'actionpoint'):
			if hasattr(action, 'orientation'):
				return action.procedure, (self,), {}, action.actionpoint + (action.orientation,)
			else:
				return action.procedure, (self,), {}, action.actionpoint + (None,)
		else:
			return action.procedure, (self,), {}, None

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

		# Return if there is no path available		
		if path_not_found:
			self.log('no path found')
			wheeledbase.stop()
			time.sleep(1)
			return False

		# Pure Pursuit configuration
		if math.cos(math.atan2(path[1][1] - path[0][1], path[1][0] - path[0][0]) - theta_in) >= 0:
			direction = 1
		else:
			direction = -1
		wheeledbase.lookahead.set(200)
		wheeledbase.lookaheadbis.set(200)
		wheeledbase.max_linvel.set(500)
		wheeledbase.max_angvel.set(1.5)
		wheeledbase.linpos_threshold.set(5)
		wheeledbase.angpos_threshold.set(0.3)

		# Trajectory
		wheeledbase.purepursuit(path, direction={1:'forward', -1:'backward'}[direction])

		# Do mandatory procedures
		def mandatory_procedures():		
			if self.storage_mandatory:
				self.storage_procedure()
		parallel_actions = self.perform(mandatory_procedures)

		# Wait until destination is reached
		isarrived = False
		blocked = False
		while not isarrived:

			# Get current position
			x_in, y_in, theta_in = wheeledbase.get_position()

			# Check for Bornibus' position
			brother_distance = self.brother.get_distance(x_in, y_in)
			if brother_distance < 700:
				self.log('detected brother at distance: {:.0f}'.format(brother_distance))
				if self.brother.is_on_path(path):
					self.log('detected that brother is on the path')
					edges = self.brother.get_edges()
					try:
						path = self.roadmap.get_shortest_path((x_in, y_in), (x_sp, y_sp))
						self.log('follow path: [{}]'.format(', '.join('({0[0]:.0f}, {0[1]:.0f})'.format(waypoint) for waypoint in path)))
						wheeledbase.purepursuit(path, direction={1:'forward', -1:'backward'}[direction])
					except RuntimeError:
						path_not_found = True
					path_not_found |= self.brother.is_on_path(path)
					if path_not_found:
						self.log('no path found')
						wheeledbase.stop()
						time.sleep(1)
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

	def start_procedure(self):
		self.log('start the final countdown')
		time.sleep(90)
		self.log('one small step for (a) robot...')
		self.launchpad.launch()

	def stop_procedure(self):
		self.wheeledbase.stop()
		self.roller.stop()
		self.rollerarm.hold(0)
		self.ballzooka.stop()

	def storage_procedure(self):
		self.log('store gathered minerals')
		try:
			if self.stored_minerals <= 5:
				self.rollerarm.goto(self.minerals_05_storage_position, 400, timeout=3)
			elif self.stored_minerals <= 10:
				self.rollerarm.goto(self.minerals_10_storage_position, 400, timeout=3)
			else:
				self.rollerarm.goto(self.minerals_15_storage_position, 400, timeout=3)
		except RuntimeError:
			self.log('roller arm could not reach the storage position')
		self.storage_mandatory = False


class DelayAction:
	def __init__(self, duration):
		self.duration = duration
	
	def procedure(self, murray):
		murray.log('wait for {} seconds'.format(self.duration))
		time.sleep(self.duration)
		murray.automatestep += 1


class GatherSmallCraterAction:
	def __init__(self, geogebra, major, minor):
		self.actionpoint = geogebra.get('crater_{{{}, action, {}}}'.format(major, minor))
		self.gatherpoint = geogebra.get('crater_{{{}, action, {}, 1}}'.format(major, minor))
		self.endingpoint = geogebra.get('crater_{{{}, action, {}, 2}}'.format(major, minor))
		self.orientation = math.atan2(self.gatherpoint[1] - self.actionpoint[1], self.gatherpoint[0] - self.actionpoint[0])
		
		crater_center = geogebra.get('crater_{{{}, center}}'.format(major))
		self.entry_lookahead  = math.hypot(crater_center[0] - self.actionpoint[0], crater_center[1] - self.actionpoint[1])
		self.gather_lookahead = math.hypot(crater_center[0] - self.gatherpoint[0], crater_center[1] - self.gatherpoint[1])

	def procedure(self, murray):
		murray.log('gather small crater')
		wheeledbase = murray.wheeledbase
		rollerarm   = murray.rollerarm
		roller      = murray.roller

		# In any case
		murray.storage_mandatory = True

		# Lower the roller arm and start the roller
		murray.log('lower the roller arm to the entry position')
		try:
			rollerarm.enter()
		except RuntimeError:
			murray.log('blocked while lowering the roller arm')
			rollerarm.hold(0)
		roller.gather()

		# Go to the gathering point
		murray.log('goto the gathering point')
		wheeledbase.lookahead.set(self.entry_lookahead)
		wheeledbase.lookaheadbis.set(self.entry_lookahead)
		try:
			wheeledbase.purepursuit([wheeledbase.get_position()[:2], self.gatherpoint], 'forward', self.orientation)
			wheeledbase.wait()
		except RuntimeError:
			murray.log('blocked on the path')
			wheeledbase.set_velocities(-100, 0)
			time.sleep(1)
			try:
				wheeledbase.purepursuit([wheeledbase.get_position()[:2], self.gatherpoint], 'forward', self.orientation)
				wheeledbase.wait()
			except RuntimeError:
				murray.log('blocked again')
				return
		
		# Lower the roller arm inside the crater
		murray.log('lower the roller arm to the gathering position')
		try:
			rollerarm.gather()
		except RuntimeError:
			murray.log('blocked while lowering the roller arm')

		# Get to the bottom of the crater
		murray.log('get to the bottom of the crater')
		wheeledbase.max_linvel.set(200)
		wheeledbase.lookahead.set(self.gather_lookahead)
		wheeledbase.lookaheadbis.set(self.gather_lookahead)
		try:
			wheeledbase.purepursuit([wheeledbase.get_position()[:2], self.endingpoint], 'forward', self.orientation)
			wheeledbase.wait()
		except RuntimeError:
			murray.log('blocked while going to the bottom of the crater')

		# Update stored minerals number
		murray.stored_minerals += 5

		# Get off the crater
		murray.log('get off the crater')
		wheeledbase.max_linvel.set(400)
		wheeledbase.lookahead.set(200)
		wheeledbase.lookaheadbis.set(200)
		while True:
			try:
				wheeledbase.purepursuit([wheeledbase.get_position()[:2], self.actionpoint], 'backward', self.orientation + math.pi)
				wheeledbase.wait()
			except RuntimeError:
				murray.log('blocked while getting off the crater')
				wheeledbase.set_velocities(100, 0)
				time.sleep(0.5)
			else:
				return


class GatherBigCraterAction:
	def __init__(self, geogebra, major, minor):
		self.major = major
		self.minor = minor

		self.actionpoint = geogebra.get('crater_{{{}, action}}'.format(major))
		self.actionpointbis = geogebra.get('crater_{{{}, action, {}}}'.format(major, minor))
		self.entrypoint  = geogebra.get('crater_{{{}, action, {}, 1}}'.format(major, minor))
		self.gatherpoint = geogebra.get('crater_{{{}, action, {}, 2}}'.format(major, minor))
		self.endingpoint = geogebra.get('crater_{{{}, action, {}, 3}}'.format(major, minor))
		self.orientation = None
		self.orientationbis = math.atan2(self.gatherpoint[1] - self.entrypoint[1], self.gatherpoint[0] - self.entrypoint[0])

		crater_center = geogebra.get('crater_{{{}, center}}'.format(major))
		self.entry_lookahead  = math.hypot(crater_center[0] - self.actionpoint[0], crater_center[1] - self.actionpoint[1])
		self.gather_lookahead = math.hypot(crater_center[0] - self.gatherpoint[0], crater_center[1] - self.gatherpoint[1])

	def procedure(self, murray):
		murray.log('gather big crater')
		wheeledbase = murray.wheeledbase
		rollerarm   = murray.rollerarm
		roller      = murray.roller

		# In any case
		murray.storage_mandatory = True

		# Goto the entry point
		try:
			murray.log('get ready to enter the crater')
			wheeledbase.purepursuit([wheeledbase.get_position()[:2], self.actionpointbis, self.entrypoint], 'backward')
			wheeledbase.wait()
		except RuntimeError:
			murray.log('blocked on the path')
			wheeledbase.set_velocities(100, 0)
			time.sleep(0.5)
			try:
				wheeledbase.purepursuit([wheeledbase.get_position()[:2], self.actionpointbis, self.entrypoint], 'backward')
				wheeledbase.wait()
			except RuntimeError:
				murray.log('blocked again')
				return

		# Get a higher precision if close to the rocket
		default_angpos_threshold = wheeledbase.angpos_threshold.get()
		if self.minor == 'a':
			wheeledbase.angpos_threshold.set(0.1)

		# Ensure it is properly oriented
		try:
			murray.log('turn on the spot')
			wheeledbase.turnonthespot(self.orientationbis)
			wheeledbase.wait()
		except RuntimeError:
			murray.log('blocked while turning on the spot')
		wheeledbase.angpos_threshold.set(default_angpos_threshold)

		# Lower the roller arm and start the roller
		murray.log('lower the roller arm to the entry position')
		try:
			rollerarm.enter()
		except RuntimeError:
			murray.log('blocked while lowering the roller arm')
			rollerarm.hold(0)
		roller.gather()

		# Go to the gathering point
		murray.log('goto the gathering point')
		wheeledbase.lookahead.set(self.entry_lookahead)
		wheeledbase.lookaheadbis.set(self.entry_lookahead)
		try:
			wheeledbase.purepursuit([wheeledbase.get_position()[:2], self.gatherpoint], 'forward', self.orientation)
			wheeledbase.wait()
		except RuntimeError:
			murray.log('blocked on the path')
			wheeledbase.set_velocities(-100, 0)
			time.sleep(1)
			try:
				wheeledbase.purepursuit([wheeledbase.get_position()[:2], self.gatherpoint], 'forward', self.orientation)
				wheeledbase.wait()
			except RuntimeError:
				murray.log('blocked again')
				return
		
		# Lower the roller arm inside the crater
		murray.log('lower the roller arm to the gathering position')
		try:
			rollerarm.gather()
		except RuntimeError:
			murray.log('blocked while lowering the roller arm')

		# Get to the bottom of the crater
		murray.log('get to the bottom of the crater')
		wheeledbase.max_linvel.set(200)
		wheeledbase.lookahead.set(self.gather_lookahead)
		wheeledbase.lookaheadbis.set(self.gather_lookahead)
		try:
			wheeledbase.purepursuit([wheeledbase.get_position()[:2], self.endingpoint], 'forward', self.orientationbis)
			wheeledbase.wait()
		except RuntimeError:
			murray.log('blocked while going to the bottom of the crater')
			wheeledbase.set_velocities(-100, 0)
			time.sleep(0.5)
			try:
				wheeledbase.purepursuit([wheeledbase.get_position()[:2], self.endingpoint], 'forward', self.orientationbis)
				wheeledbase.wait()
			except RuntimeError:
				murray.log('blocked again')

		# Update stored minerals number
		murray.stored_minerals += 10

		# Get off the crater
		murray.log('get off the crater')
		wheeledbase.max_linvel.set(400)
		wheeledbase.lookahead.set(200)
		wheeledbase.lookaheadbis.set(200)
		while True:
			try:
				wheeledbase.purepursuit([wheeledbase.get_position()[:2], self.entrypoint], 'backward', self.orientationbis + math.pi)
				wheeledbase.wait()
			except RuntimeError:
				murray.log('blocked while getting off the crater')
				wheeledbase.set_velocities(100, 0)
				time.sleep(0.5)
			else:
				return


class FireMineralsAction:
	def __init__(self, geogebra, major, minor):
		self.actionpoint = geogebra.get('hold_{{{}, action, {}}}'.format(major, minor))
		self.firingpoint = geogebra.get('hold_{{{}, action, {}, 1}}'.format(major, minor))
		self.orientation = math.pi + math.atan2(self.firingpoint[1] - self.actionpoint[1], self.firingpoint[0] - self.actionpoint[0])
		self.hold = geogebra.get('hold_{{{}}}'.format(major))

	def procedure(self, murray):
		murray.log('fire minerals')
		wheeledbase = murray.wheeledbase
		rollerarm   = murray.rollerarm
		roller      = murray.roller
		ballzooka   = murray.ballzooka

		# Goto the firing point
		murray.log('go to the firing point')
		try:
			wheeledbase.purepursuit([wheeledbase.get_position()[:2], self.firingpoint], 'backward')
			wheeledbase.wait()
		except RuntimeError:
			murray.log('blocked on the path')
		
		# Ensure it is properly oriented
		murray.log('turn on the spot')
		default_angpos_threshold = wheeledbase.angpos_threshold.get()
		wheeledbase.angpos_threshold.set(0.1)
		try:
			x_in, y_in = wheeledbase.get_position()[:2]
			theta_sp = math.atan2(self.hold[1] - y_in, self.hold[0] - x_in) + math.pi
			wheeledbase.turnonthespot(theta_sp)
			wheeledbase.wait()
		except RuntimeError:
			murray.log('blocked while turning on the spot')
			wheeledbase.set_velocities(100, 0)
			time.sleep(0.5)
			try:
				x_in, y_in = wheeledbase.get_position()[:2]
				theta_sp = math.atan2(self.hold[1] - y_in, self.hold[0] - x_in) + math.pi
				wheeledbase.turnonthespot(theta_sp)
				wheeledbase.wait()
			except RuntimeError:
				murray.log('blocked again')
		wheeledbase.angpos_threshold.set(default_angpos_threshold)

		# Start firing
		murray.log('start firing')
		ballzooka.fire()

		# Define jiggling procedure
		def jiggling_procedure(low, high):
			while True:
				try: rollerarm.goto(low, 1023, threshold=1, timeout=1)
				except RuntimeError: pass
				try: rollerarm.goto(high, 1023, threshold=1, timeout=1)
				except RuntimeError: pass

		# Raise the roller arm slowly
		if murray.stored_minerals > 5:
			murray.log('raise the roller arm')
			try:
				raising_velocity = (murray.minerals_05_storage_position - rollerarm.get_position()) / (murray.firing_cadency * (murray.stored_minerals - 5))
				rollerarm.goto(murray.minerals_05_storage_position, round(1.7 * raising_velocity), timeout=2 + murray.firing_cadency * (murray.stored_minerals - 5))
			except RuntimeError:
				murray.log('blocked while raising the roller arm')
				try: rollerarm.goto(180)
				except RuntimeError: pass
				jiggling = murray.perform(jiggling_procedure, args=(160, 220))
				time.sleep(4)
				murray.interrupt(jiggling)
				try:
					raising_velocity = (murray.minerals_05_storage_position - rollerarm.get_position()) / (murray.firing_cadency * (murray.stored_minerals - 5))
					rollerarm.goto(murray.minerals_05_storage_position, round(1.7 * raising_velocity), timeout=2 + murray.firing_cadency * (murray.stored_minerals - 5))
				except RuntimeError:
					murray.log('blocked again')
					roller.stop()
					ballzooka.stop()
					murray.stored_minerals -= 5
					return

		# Start jiggling the roller arm
		jiggling = murray.perform(jiggling_procedure, args=(220, 300))
		time.sleep(4)
		murray.interrupt(jiggling)

		# Stop firing
		murray.log('stop firing')
		roller.stop()
		ballzooka.stop()
		murray.stored_minerals = 0


class StrikeModuleAction:
	def __init__(self, geogebra, major, minor):
		self.actionpoint = geogebra.get('module_{{{}, action, {}}}'.format(major, minor))
		self.strikepoint = geogebra.get('module_{{{}, action, {}, 1}}'.format(major, minor))
		self.orientation = math.atan2(self.strikepoint[1] - self.actionpoint[1], self.strikepoint[0] - self.actionpoint[0])

	def procedure(self, murray):
		murray.log('strike module')
		wheeledbase = murray.wheeledbase
		rollerarm   = murray.rollerarm

		# Put the roller arm in a medium position
		try:
			rollerarm.goto(150, 1023)
		except RuntimeError:
			murray.log('blocked while putting the roller arm in the strike position')

		# Run at the module
		try:
			wheeledbase.goto(*self.strikepoint)
		except RuntimeError:
			murray.log('blocked while striking the module')
		
		# Turn a little
		wheeledbase.set_velocities(0, 0.5)
		time.sleep(0.5)

		# Raise the roller arm
		try:
			rollerarm.close()
		except RuntimeError:
			pass

