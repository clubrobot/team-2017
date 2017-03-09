#!/usr/bin/env python3
#-*- coding: utf-8 -*-

import time
import math

from common.serialtalks   import BYTE, INT, FLOAT
from common.modulesrouter import Module

# Wheeled base instructions

SET_OPENLOOP_VELOCITIES_OPCODE  = 0x04

GET_CODEWHEELS_COUNTERS_OPCODE  = 0x0D

SET_VELOCITIES_OPCODE   = 0x06

START_TRAJECTORY_OPCODE = 0x07
TRAJECTORY_ENDED_OPCODE = 0x08

SET_POSITION_OPCODE     = 0x0A
GET_POSITION_OPCODE     = 0x0B
GET_VELOCITIES_OPCODE   = 0x0C

SET_PARAMETER_VALUE_OPCODE  = 0x0E
GET_PARAMETER_VALUE_OPCODE  = 0x0F

LEFTWHEEL_RADIUS_ID          = 0x10
LEFTWHEEL_CONSTANT_ID        = 0x11
RIGHTWHEEL_RADIUS_ID         = 0x20
RIGHTWHEEL_CONSTANT_ID       = 0x21
LEFTCODEWHEEL_RADIUS_ID        = 0x40
LEFTCODEWHEEL_COUNTSPERREV_ID  = 0x41
RIGHTCODEWHEEL_RADIUS_ID       = 0x50
RIGHTCODEWHEEL_COUNTSPERREV_ID = 0x51
ODOMETRY_AXLETRACK_ID        = 0x60
VELOCITYCONTROL_AXLETRACK_ID = 0x80
VELOCITYCONTROL_MAXLINACC_ID = 0x81
VELOCITYCONTROL_MAXLINDEC_ID = 0x82
VELOCITYCONTROL_MAXANGACC_ID = 0x83
VELOCITYCONTROL_MAXANGDEC_ID = 0x84
LINVELPID_KP_ID              = 0xA0
LINVELPID_KI_ID              = 0xA1
LINVELPID_KD_ID              = 0xA2
LINVELPID_MINOUTPUT_ID       = 0xA3
LINVELPID_MAXOUTPUT_ID       = 0xA4
ANGVELPID_KP_ID              = 0xB0
ANGVELPID_KI_ID              = 0xB1
ANGVELPID_KD_ID              = 0xB2
ANGVELPID_MINOUTPUT_ID       = 0xB3
ANGVELPID_MAXOUTPUT_ID       = 0xB4


class WheeledBase(Module):
	def __init__(self, parent, uuid='wheeledbase'):
		Module.__init__(self, parent, uuid)

	def set_openloop_velocities(self, left, right):
		self.send(SET_OPENLOOP_VELOCITIES_OPCODE, FLOAT(left), FLOAT(right))

	def get_codewheels_counter(self, **kwargs):
		output = self.execute(GET_CODEWHEELS_COUNTERS_OPCODE, **kwargs)
		left, right = output.read(LONG, LONG)
		return left, right

	def set_velocities(self, linear_velocity, angular_velocity):
		self.send(SET_VELOCITIES_OPCODE, FLOAT(linear_velocity), FLOAT(angular_velocity))

	def start_trajectory(self, x, y, theta):
		self.send(START_TRAJECTORY_OPCODE, FLOAT(x), FLOAT(y), FLOAT(theta))
	
	def trajectory_ended(self, **kwargs):
		output = self.execute(TRAJECTORY_ENDED_OPCODE, **kwargs)
		trajectory_ended = output.read(BYTE)
		return bool(trajectory_ended)

	def goto(self, x, y, theta=None, **kwargs):
		if theta is None:
			current_x, current_y, current_theta = self.get_position(**kwargs)
			theta = math.atan2(y - current_y, x - current_x)
		self.start_trajectory(x, y, theta)
		while not self.trajectory_ended(**kwargs):
			time.sleep(0.1)

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

