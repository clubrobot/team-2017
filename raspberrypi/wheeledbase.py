#!/usr/bin/env python3
#-*- coding: utf-8 -*-

import time
import math

from serialtalks import BYTE, INT, LONG, FLOAT
from components import SerialTalksProxy

# Instructions

SET_OPENLOOP_VELOCITIES_OPCODE  = 0x04

GET_CODEWHEELS_COUNTERS_OPCODE  = 0x0D

SET_VELOCITIES_OPCODE           = 0x06

START_PUREPURSUIT_OPCODE        = 0x07
START_TURNONTHESPOT_OPCODE      = 0x09

POSITION_REACHED_OPCODE         = 0x08

SET_POSITION_OPCODE	            = 0x0A
GET_POSITION_OPCODE	            = 0x0B
GET_VELOCITIES_OPCODE           = 0x0C

SET_PARAMETER_VALUE_OPCODE      = 0x0E
GET_PARAMETER_VALUE_OPCODE      = 0x0F

RESET_PUREPURSUIT_OPCODE        = 0x10
ADD_PUREPURSUIT_WAYPOINT_OPCODE = 0x11

LEFTWHEEL_RADIUS_ID	            = 0x10
LEFTWHEEL_CONSTANT_ID           = 0x11
LEFTWHEEL_MAXPWM_ID             = 0x12
RIGHTWHEEL_RADIUS_ID            = 0x20
RIGHTWHEEL_CONSTANT_ID          = 0x21
RIGHTWHEEL_MAXPWM_ID            = 0x22
LEFTCODEWHEEL_RADIUS_ID	        = 0x40
LEFTCODEWHEEL_COUNTSPERREV_ID   = 0x41
RIGHTCODEWHEEL_RADIUS_ID        = 0x50
RIGHTCODEWHEEL_COUNTSPERREV_ID  = 0x51
ODOMETRY_AXLETRACK_ID           = 0x60
ODOMETRY_SLIPPAGE_ID            = 0x61
VELOCITYCONTROL_AXLETRACK_ID    = 0x80
VELOCITYCONTROL_MAXLINACC_ID    = 0x81
VELOCITYCONTROL_MAXLINDEC_ID    = 0x82
VELOCITYCONTROL_MAXANGACC_ID    = 0x83
VELOCITYCONTROL_MAXANGDEC_ID    = 0x84
VELOCITYCONTROL_SPINSHUTDOWN_ID = 0x85
LINVELPID_KP_ID                 = 0xA0
LINVELPID_KI_ID                 = 0xA1
LINVELPID_KD_ID                 = 0xA2
LINVELPID_MINOUTPUT_ID          = 0xA3
LINVELPID_MAXOUTPUT_ID          = 0xA4
ANGVELPID_KP_ID                 = 0xB0
ANGVELPID_KI_ID                 = 0xB1
ANGVELPID_KD_ID                 = 0xB2
ANGVELPID_MINOUTPUT_ID	        = 0xB3
ANGVELPID_MAXOUTPUT_ID	        = 0xB4
POSITIONCONTROL_LINVELKP_ID     = 0xD0
POSITIONCONTROL_ANGVELKP_ID     = 0xD1
POSITIONCONTROL_LINVELMAX_ID    = 0xD2
POSITIONCONTROL_ANGVELMAX_ID    = 0xD3
POSITIONCONTROL_LINPOSTHRESHOLD_ID  = 0xD4
POSITIONCONTROL_ANGPOSTHRESHOLD_ID  = 0xD5
PUREPURSUIT_LOOKAHEAD_ID        = 0xE0
PUREPURSUIT_LOOKAHEADBIS_ID     = 0xE2


class WheeledBase(SerialTalksProxy):

	class Parameter():
		def __init__(self, parent, id, type):
			self.parent = parent
			self.id   = id
			self.type = type
		def get(self): return self.parent.get_parameter_value(self.id, self.type)
		def set(self, value): self.parent.set_parameter_value(self.id, value, self.type)

	def __init__(self, parent, uuid='wheeledbase'):
		SerialTalksProxy.__init__(self, parent, uuid)

		self.left_wheel_radius   = WheeledBase.Parameter(self, LEFTWHEEL_RADIUS_ID, FLOAT)
		self.left_wheel_constant = WheeledBase.Parameter(self, LEFTWHEEL_CONSTANT_ID, FLOAT)
		self.left_wheel_maxPWM   = WheeledBase.Parameter(self, LEFTWHEEL_MAXPWM_ID, FLOAT)

		self.right_wheel_radius   = WheeledBase.Parameter(self, RIGHTWHEEL_RADIUS_ID, FLOAT)
		self.right_wheel_constant = WheeledBase.Parameter(self, RIGHTWHEEL_CONSTANT_ID, FLOAT)
		self.right_wheel_maxPWM   = WheeledBase.Parameter(self, RIGHTWHEEL_MAXPWM_ID, FLOAT)

		self.left_codewheel_radius         = WheeledBase.Parameter(self, LEFTCODEWHEEL_RADIUS_ID, FLOAT)
		self.left_codewheel_counts_per_rev = WheeledBase.Parameter(self, LEFTCODEWHEEL_COUNTSPERREV_ID, LONG)

		self.right_codewheel_radius         = WheeledBase.Parameter(self, RIGHTCODEWHEEL_RADIUS_ID, FLOAT)
		self.right_codewheel_counts_per_rev = WheeledBase.Parameter(self, RIGHTCODEWHEEL_COUNTSPERREV_ID, LONG)

		self.codewheels_axletrack = WheeledBase.Parameter(self, ODOMETRY_AXLETRACK_ID, FLOAT)
		self.odometry_slippage    = WheeledBase.Parameter(self, ODOMETRY_SLIPPAGE_ID, FLOAT)

		self.wheels_axletrack = WheeledBase.Parameter(self, VELOCITYCONTROL_AXLETRACK_ID, FLOAT)
		self.max_linacc = WheeledBase.Parameter(self, VELOCITYCONTROL_MAXLINACC_ID, FLOAT)
		self.max_lindec = WheeledBase.Parameter(self, VELOCITYCONTROL_MAXLINDEC_ID, FLOAT)
		self.max_angacc = WheeledBase.Parameter(self, VELOCITYCONTROL_MAXANGACC_ID, FLOAT)
		self.max_angdec = WheeledBase.Parameter(self, VELOCITYCONTROL_MAXANGDEC_ID, FLOAT)
		self.spin_shutdown = WheeledBase.Parameter(self, VELOCITYCONTROL_SPINSHUTDOWN_ID, BYTE)
		
		self.linvel_KP = WheeledBase.Parameter(self, LINVELPID_KP_ID, FLOAT)
		self.linvel_KI = WheeledBase.Parameter(self, LINVELPID_KI_ID, FLOAT)
		self.linvel_KD = WheeledBase.Parameter(self, LINVELPID_KD_ID, FLOAT)

		self.angvel_KP = WheeledBase.Parameter(self, ANGVELPID_KP_ID, FLOAT)
		self.angvel_KI = WheeledBase.Parameter(self, ANGVELPID_KI_ID, FLOAT)
		self.angvel_KD = WheeledBase.Parameter(self, ANGVELPID_KD_ID, FLOAT)

		self.linpos_KP  = WheeledBase.Parameter(self, POSITIONCONTROL_LINVELKP_ID, FLOAT)
		self.angpos_KP  = WheeledBase.Parameter(self, POSITIONCONTROL_ANGVELKP_ID, FLOAT)
		self.max_linvel = WheeledBase.Parameter(self, POSITIONCONTROL_LINVELMAX_ID, FLOAT)
		self.max_angvel = WheeledBase.Parameter(self, POSITIONCONTROL_ANGVELMAX_ID, FLOAT)
		self.linpos_threshold = WheeledBase.Parameter(self, POSITIONCONTROL_LINPOSTHRESHOLD_ID, FLOAT)
		self.angpos_threshold = WheeledBase.Parameter(self, POSITIONCONTROL_ANGPOSTHRESHOLD_ID, FLOAT)
		
		self.lookahead    = WheeledBase.Parameter(self, PUREPURSUIT_LOOKAHEAD_ID, FLOAT)
		self.lookaheadbis = WheeledBase.Parameter(self, PUREPURSUIT_LOOKAHEADBIS_ID, FLOAT)

	def set_openloop_velocities(self, left, right):
		self.send(SET_OPENLOOP_VELOCITIES_OPCODE, FLOAT(left), FLOAT(right))

	def get_codewheels_counter(self, **kwargs):
		output = self.execute(GET_CODEWHEELS_COUNTERS_OPCODE, **kwargs)
		left, right = output.read(LONG, LONG)
		return left, right

	def set_velocities(self, linear_velocity, angular_velocity):
		print('set_velocities', linear_velocity, angular_velocity)
		self.send(SET_VELOCITIES_OPCODE, FLOAT(linear_velocity), FLOAT(angular_velocity))

	def purepursuit(self, waypoints, direction='forward', finalangle=None, lookahead=None, lookaheadbis=None, linvelmax=None, angvelmax=None):
		print('purepursuit', waypoints)
		if len(waypoints) < 2:
			raise ValueError('not enough waypoints')
		self.send(RESET_PUREPURSUIT_OPCODE)
		for x, y in waypoints:
			self.send(ADD_PUREPURSUIT_WAYPOINT_OPCODE, FLOAT(x), FLOAT(y))
		if lookahead is not None:
			self.set_parameter_value(PUREPURSUIT_LOOKAHEAD_ID, lookahead, FLOAT)
		if lookaheadbis is not None:
			self.set_parameter_value(PUREPURSUIT_LOOKAHEADBIS_ID, lookaheadbis, FLOAT)
		if linvelmax is not None:
			self.set_parameter_value(POSITIONCONTROL_LINVELMAX_ID, linvelmax, FLOAT)
		if angvelmax is not None:
			self.set_parameter_value(POSITIONCONTROL_ANGVELMAX_ID, angvelmax, FLOAT)
		if finalangle is None:
			finalangle = math.atan2(waypoints[-1][1] - waypoints[-2][1], waypoints[-1][0] - waypoints[-2][0])
		self.send(START_PUREPURSUIT_OPCODE, BYTE({'forward':0, 'backward':1}[direction]), FLOAT(finalangle))

	def turnonthespot(self, theta):
		self.send(START_TURNONTHESPOT_OPCODE, FLOAT(theta))

	def isarrived(self, **kwargs):
		output = self.execute(POSITION_REACHED_OPCODE, **kwargs)
		isarrived, spinurgency = output.read(BYTE, BYTE)
		if bool(spinurgency):
			raise RuntimeError('spin urgency')
		return bool(isarrived)

	def wait(self, timestep=0.1, **kwargs):
		while not self.isarrived(**kwargs):
			time.sleep(timestep)

	def goto(self, x, y, theta=None, direction=None, **kwargs):
		# Compute the preferred direction if not set
		if direction is None:
			x0, y0, theta0 = self.get_position()
			if math.cos(math.atan2(y - y0, x - x0) - theta0) >= 0:
				direction = 'forward'
			else:
				direction = 'backward'
		
		# Go to the setpoint position
		self.purepursuit([self.get_position()[0:2], (x, y)], direction)
		self.wait(**kwargs)
		
		# Get the setpoint orientation
		if theta is not None:
			self.turnonthespot(theta)
			self.wait(**kwargs)

	def stop(self):
		self.set_openloop_velocities(0, 0)

	def set_position(self, x, y, theta):
		self.send(SET_POSITION_OPCODE, FLOAT(x), FLOAT(y), FLOAT(theta))
	
	def reset(self):
		self.set_position(0, 0, 0)

	def get_position(self, **kwargs):
		output = self.execute(GET_POSITION_OPCODE, **kwargs)
		x, y, theta = output.read(FLOAT, FLOAT, FLOAT)
		return x, y, theta
	
	def get_velocities(self, **kwargs):
		output = self.execute(GET_VELOCITIES_OPCODE, **kwargs)
		linvel, angvel = output.read(FLOAT, FLOAT)
		return linvel, angvel

	def set_parameter_value(self, id, value, valuetype):
		self.send(SET_PARAMETER_VALUE_OPCODE, BYTE(id), valuetype(value))
	
	def get_parameter_value(self, id, valuetype):
		output = self.execute(GET_PARAMETER_VALUE_OPCODE, BYTE(id))
		value = output.read(valuetype)
		return value
