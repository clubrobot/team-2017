#!/usr/bin/env python3
#-*- coding: utf-8 -*-

import time
import math

from common.serialtalks   import BYTE, INT, LONG, FLOAT
from common.modulesrouter import Module

# Wheeled base instructions

SET_OPENLOOP_VELOCITIES_OPCODE  = 0x04

GET_CODEWHEELS_COUNTERS_OPCODE  = 0x0D

SET_VELOCITIES_OPCODE   = 0x06

START_TRAJECTORY_OPCODE = 0x07
TRAJECTORY_ENDED_OPCODE = 0x08

SET_POSITION_OPCODE	 = 0x0A
GET_POSITION_OPCODE	 = 0x0B
GET_VELOCITIES_OPCODE   = 0x0C

SET_PARAMETER_VALUE_OPCODE  = 0x0E
GET_PARAMETER_VALUE_OPCODE  = 0x0F

LEFTWHEEL_RADIUS_ID		  = 0x10
LEFTWHEEL_CONSTANT_ID		= 0x11
RIGHTWHEEL_RADIUS_ID		 = 0x20
RIGHTWHEEL_CONSTANT_ID	   = 0x21
LEFTCODEWHEEL_RADIUS_ID		= 0x40
LEFTCODEWHEEL_COUNTSPERREV_ID  = 0x41
RIGHTCODEWHEEL_RADIUS_ID	   = 0x50
RIGHTCODEWHEEL_COUNTSPERREV_ID = 0x51
ODOMETRY_AXLETRACK_ID		= 0x60
VELOCITYCONTROL_AXLETRACK_ID = 0x80
VELOCITYCONTROL_MAXLINACC_ID = 0x81
VELOCITYCONTROL_MAXLINDEC_ID = 0x82
VELOCITYCONTROL_MAXANGACC_ID = 0x83
VELOCITYCONTROL_MAXANGDEC_ID = 0x84
LINVELPID_KP_ID			  = 0xA0
LINVELPID_KI_ID			  = 0xA1
LINVELPID_KD_ID			  = 0xA2
LINVELPID_MINOUTPUT_ID	   = 0xA3
LINVELPID_MAXOUTPUT_ID	   = 0xA4
ANGVELPID_KP_ID			  = 0xB0
ANGVELPID_KI_ID			  = 0xB1
ANGVELPID_KD_ID			  = 0xB2
ANGVELPID_MINOUTPUT_ID	   = 0xB3
ANGVELPID_MAXOUTPUT_ID	   = 0xB4

# Modules collector instructions

_WRITE_DISPENSER_OPCODE	=  0x04
_WRITE_GRIP_OPCODE		 =  0X05
_IS_UP_OPCODE			  =  0x08
_IS_DOWN_OPCODE			=  0x09
_SET_MOTOR_VELOCITY_OPCODE =  0x0C


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

	def start_trajectory(self, waypoints):
		args = [INT(len(waypoints))]
		for x, y, theta in waypoints:
			args += [FLOAT(x), FLOAT(y), FLOAT(theta)]
		self.send(START_TRAJECTORY_OPCODE, *args)

	def trajectory_ended(self, **kwargs):
		output = self.execute(TRAJECTORY_ENDED_OPCODE, **kwargs)
		trajectory_ended = output.read(BYTE)
		return bool(trajectory_ended)

	def goto(self, x, y, theta=None, **kwargs):
		if theta is None:
			current_x, current_y, current_theta = self.get_position(**kwargs)
			theta = math.atan2(y - current_y, x - current_x)
		self.start_trajectory([(x, y, theta)])
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





class ModulesGripper(Module):	
	def __init__(self, parent, uuid='modulescollector'):
		Module.__init__(self, parent, uuid)
		self.high_gripOpenAngle = 147
		self.lowGripOpenAngle = 80
		self.closeGripAngle = 5
		self.gripCylinderAngle = 45

	def set_position(self,a):
		self.send(_WRITE_GRIP_OPCODE, INT(a))

	def open_up(self):
		tempPosCommand = self.gripCylinderAngle
		step = 1
		time = 1 
		while tempPosCommand<=self.highGripOpenAngle :
			self.set_position(round(tempPosCommand))
			tempPosCommand += step
			time.sleep(time/(self.highGripOpenAngle-self.gripCylinderAngle))

	def open_low(self):
		self.set_position(self.lowGripOpenAngle)

	def close(self):
		self.set_position(self.closeGripAngle)


class ModulesDispenser(Module):
	def __init__(self, parent, uuid='modulescollector'):
		Module.__init__(self, parent, uuid)
		self.openDispenserAngle = 133
		self.closeDispenserAngle = 0
	
	def set_position(self,a): 
		self.send(_WRITE_DISPENSER_OPCODE, INT(a))

	def open(self):
		self.set_position(self.openDispenserAngle)

	def close(self):
		self.set_position(self.closeDispenserAngle)
		time.sleep(1)
		self.set_position(-1)



class ModulesMotor(Module):
	def __init__(self, parent, uuid='modulescollector'):
		Module.__init__(self, parent, uuid)
		self.climbingVelocity = -400 
		self.goingDownVelocity = 300
	
	def get_high(self):
		output = self.execute(_IS_UP_OPCODE)
		isup = output.read(BYTE)
		return bool(isup)

	def get_down(self):
		output = self.execute(_IS_DOWN_OPCODE)
		isdown = output.read(BYTE)
		return bool(isdown)
	
	def set_velocity(self, a):
		self.send(_SET_MOTOR_VELOCITY_OPCODE, FLOAT(a))

	def toup(self):
		self.set_velocity(self.climbingVelocity)

	def todown(self):
		self.set_velocity(self.goingDownVelocity)