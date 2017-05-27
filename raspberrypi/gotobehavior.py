#!/usr/bin/env python3
#-*- coding: utf-8 -*-

import time
import math


class GotoBehavior:

	def __init__(self, robot):
		self.robot = robot
		
		# SerialTalks proxies
		self.wheeledbase = robot.wheeledbase
		self.sensors     = robot.sensors

		# Bluetooth support
		self.brother = robot.brother

		# Behavior parameter
		self.max_linvel = 400
		self.max_angvel = 2.0
		self.lookahead = 200
		self.linpos_threshold = 3
		self.angpos_threshold = 0.1
		self.brother_avoidance = False
		self.brother_distance = 200

	def log(*arg, **kwargs):
		return self.robot.log(*args, **kwargs)

	def __call__(self, x_sp, y_sp, theta_sp):
		# Get current position
		x_in, y_in, theta_in = self.wheeledbase.get_position()

		# Priori avoidance
		if self.brother_avoidance:
			for edge in self.brother.get_edges():
			 	self.roadmap.cut_edges(edge)

		# Pathfinding
		path_not_found = False
		try:
			path = self.roadmap.get_shortest_path((x_in, y_in), (x_sp, y_sp))
			self.log('follow path: [{}]'.format(', '.join('({0[0]:.0f}, {0[1]:.0f})'.format(waypoint) for waypoint in path)))
		except RuntimeError:
			path_not_found = True
		if not self.brother_avoidance:
			path_not_found |= self.brother.is_on_path(path)

		# Pure Pursuit configuration
		if math.cos(math.atan2(path[1][1] - path[0][1], path[1][0] - path[0][0]) - theta_in) >= 0:
			direction = 1
		else:
			direction = -1
		self.wheeledbase.lookahead.set(self.lookahead)
		self.wheeledbase.lookaheadbis.set(self.lookahead)
		self.wheeledbase.max_linvel.set(self.max_linvel)
		self.wheeledbase.max_angvel.set(self.max_angvel)
		self.wheeledbase.linpos_threshold.set(self.linpos_threshold)
		self.wheeledbase.angpos_threshold.set(self.angpos_threshold)

		# Trajectory
		self.wheeledbase.purepursuit(path, direction={1:'forward', -1:'backward'}[direction])

		# Wait until destination is reached
		isarrived = False
		blocked = False
		while not isarrived:
			# Get current position
			x_in, y_in, theta_in = self.wheeledbase.get_position()

			# Check for brother's position
			brother_distance = self.brother.get_distance(x_in, y_in)
			if brother_distance < self.brother_distance:
				self.log('detected brother at distance: {:.0f}'.format(brother_distance))
				self.log('brother shape is : [{}'.format(', '.join('({0[0]:.0f}, {0[1]:.0f})'.format(vertex) for vertex in brother.shape)))

				if self.brother.is_on_path(path):
					self.log('detected that brother is on the path')
					edges = self.brother.get_edges()
					try:
						path = self.roadmap.get_shortest_path((x_in, y_in), (x_sp, y_sp))
						self.log('follow path: [{}]'.format(', '.join('({0[0]:.0f}, {0[1]:.0f})'.format(waypoint) for waypoint in path)))
						self.wheeledbase.purepursuit(path, direction={1:'forward', -1:'backward'}[direction])
					except RuntimeError:
						path_not_found = True
					path_not_found |= self.brother.is_on_path(path)
					if path_not_found:
						self.log('no path found')
						self.wheeledbase.stop()
						return False

			# Get trajectory planner situation
			try:
				isarrived = self.wheeledbase.isarrived()
			except RuntimeError:
				self.log('blocked while following path')
				blocked = True
			
			# Handle spin urgency
			if blocked:
				self.log('go backward a little')
				self.wheeledbase.set_velocities(-direction * 100, 0)
				time.sleep(1)
				self.log('resume path')
				self.wheeledbase.purepursuit(path, direction={1:'forward', -1:'backward'}[direction])
				blocked = False
			
			# Delay
			time.sleep(0.1)
		self.log('path ended at: ({0[0]:.0f}, {0[1]:.0f}, {0[2]:.2f})'.format((x_in, y_in, theta_in)))

		# Turn on the spot
		if theta_sp is not None:
			if math.sin(theta_sp - theta_in) > 0:
				turndirection = 1
			else:
				turndirection = -1
			self.log('turn on the spot: {:.2f}'.format(theta_sp))
			self.wheeledbase.turnonthespot(theta_sp)

			# Wait until destination is reached
			isarrived = False
			blocked = False
			while not isarrived:
				# Get current position
				x_in, y_in, theta_in = self.wheeledbase.get_position():

				# Get trajectory planner situation
				try:
					isarrived = self.wheeledbase.isarrived()
				except RuntimeError:
					self.log('blocked while turning on the spot')
					blocked = True
				
				# Handle spin urgency
				if blocked:
					self.log('turn the other way')
					self.wheeledbase.set_velocities(0, -turndirection * 1.5)
					time.sleep(2)
					self.log('resume path')
					self.wheeledbase.turnonthespot(theta_sp)
					blocked = False
			
			# Delay
			time.sleep(0.1)
		self.log('rotation ended at: ({0[0]:.0f}, {0[1]:.0f}, {0[2]:.2f})'.format((x_in, y_in, theta_in)))

		# Everything is fine
		return True
