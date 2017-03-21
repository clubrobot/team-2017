#!/usr/bin/env python3
#-*- coding: utf-8 -*-

import time
import math

from common.serialtalks   import BYTE, INT, LONG, FLOAT
from common.modulesrouter import Module

# Wheeled base instructions

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

LEFTWHEEL_RADIUS_ID	            = 0x10
LEFTWHEEL_CONSTANT_ID           = 0x11
RIGHTWHEEL_RADIUS_ID            = 0x20
RIGHTWHEEL_CONSTANT_ID          = 0x21
LEFTCODEWHEEL_RADIUS_ID	        = 0x40
LEFTCODEWHEEL_COUNTSPERREV_ID   = 0x41
RIGHTCODEWHEEL_RADIUS_ID        = 0x50
RIGHTCODEWHEEL_COUNTSPERREV_ID  = 0x51
ODOMETRY_AXLETRACK_ID           = 0x60
VELOCITYCONTROL_AXLETRACK_ID    = 0x80
VELOCITYCONTROL_MAXLINACC_ID    = 0x81
VELOCITYCONTROL_MAXLINDEC_ID    = 0x82
VELOCITYCONTROL_MAXANGACC_ID    = 0x83
VELOCITYCONTROL_MAXANGDEC_ID    = 0x84
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
PUREPURSUIT_LOOKAHED_ID         = 0xE0

# Modules collector instructions

_WRITE_DISPENSER_OPCODE     = 0x04
_WRITE_GRIP_OPCODE          = 0X05
_OPEN_SLOWLY_OPCODE			= 0x06
_IS_UP_OPCODE			    = 0x08
_IS_DOWN_OPCODE			    = 0x09
_SET_MOTOR_VELOCITY_OPCODE  = 0x0C

# Ultrasonic sensors instructions

_GET_MESURE_SENSOR_OPCODE   = 0x06


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

	def purepursuit(self, waypoints):
		args = [INT(len(waypoints))] + [FLOAT(x) + FLOAT(y) for x, y in waypoints]
		self.send(START_PUREPURSUIT_OPCODE, *args)

	def turnonthespot(self, theta):
		self.send(START_TURNONTHESPOT_OPCODE, FLOAT(theta))

	def position_reached(self, **kwargs):
		output = self.execute(POSITION_REACHED_OPCODE, **kwargs)
		trajectory_ended = output.read(BYTE)
		return bool(trajectory_ended)

	def goto(self, x, y, theta=None, **kwargs):
		self.purepursuit([(x, y)])
		while not self.position_reached(**kwargs):
			time.sleep(0.1)
		if theta is not None:
			turnonthespot(theta)
			while not self.position_reached(**kwargs):
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





class ModulesGripper(Module):	
	def __init__(self, parent, uuid='modulescollector'):
		Module.__init__(self, parent, uuid)
		self.high_open_angle = 147
		self.low_open_angle = 80
		self.close_angle = 5
		self.grip_cylinder_angle = 45

	def set_position(self,a):
		self.send(_WRITE_GRIP_OPCODE, INT(a))

	def open_up(self, t):
		self.send(_OPEN_SLOWLY_OPCODE, FLOAT(t))

	def open_up(self):
		self.set_position(self.high_open_angle)

	def open_low(self):
		self.set_position(self.low_open_angle)

	def close(self):
		self.set_position(self.close_angle)


class ModulesDispenser(Module):
	def __init__(self, parent, uuid='modulescollector'):
		Module.__init__(self, parent, uuid)
		self.open_dispenser_angle = 133
		self.close_dispenser_angle = 0
	
	def set_position(self,a): 
		self.send(_WRITE_DISPENSER_OPCODE, INT(a))

	def open(self):
		self.set_position(self.open_dispenser_angle)

	def close(self):
		self.set_position(self.close_dispenser_angle)
		time.sleep(1)
		self.set_position(-1)



class ModulesElevator(Module):
	def __init__(self, parent, uuid='modulescollector'):
		Module.__init__(self, parent, uuid)
		self.climbing_Velocity = -11.1 
		self.going_down_velocity = 8
	
	def isup(self):
		output = self.execute(_IS_UP_OPCODE)
		isup = output.read(BYTE)
		return bool(isup)

	def isdown(self):
		output = self.execute(_IS_DOWN_OPCODE)
		isdown = output.read(BYTE)
		return bool(isdown)
	
	def set_velocity(self, a):
		self.send(_SET_MOTOR_VELOCITY_OPCODE, FLOAT(a))

	def go_up(self):
		self.set_velocity(self.climbing_Velocity)
		while not self.isup():
			time.sleep(0.1)

	def go_down(self):
		self.set_velocity(self.going_down_velocity)
		while not self.isdown():
			time.sleep(0.1)


class UltrasonicSensor(Module):
	def __init__(self, parent, uuid='sensors'):
		Module.__init__(self, parent, uuid)

	def get_mesure(self,**kwargs):
		output = self.execute(_GET_MESURE_SENSOR_OPCODE, **kwargs)
		ar,av=output.read(INT,INT)
		return ar,av

