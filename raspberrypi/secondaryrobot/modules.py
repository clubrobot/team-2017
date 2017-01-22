#!/usr/bin/env python3
#-*- coding: utf-8 -*-

from math import pi

from common.serialtalks   import *
from common.modulesrouter import Module

# Wheeled base instructions

SET_MOTORS_VELOCITIES_OPCODE    = 0x04

SET_VELOCITIES_OPCODE   = 0x06

GOTO_OPCODE             = 0x07

SET_POSITION_OPCODE     = 0x0A
GET_POSITION_OPCODE     = 0x0B
GET_VELOCITIES_OPCODE   = 0x0C

SET_PID_TUNINGS_OPCODE  = 0x0E
GET_PID_TUNINGS_OPCODE  = 0x0F

LINEAR_VELOCITY_PID_IDENTIFIER  = 0x02
ANGULAR_VELOCITY_PID_IDENTIFIER = 0x03
LINEAR_POSITION_PID_IDENTIFIER  = 0x04
ANGULAR_POSITION_PID_IDENTIFIER = 0x05

# Gripper instructions

SET_GRIPPER_ENABLED_OPCODE  = 0x04
SET_GRIPPER_POSITION_OPCODE = 0x05


class WheeledBase(Module):
	def __init__(self, parent, uuid='wheeledbase'):
		Module.__init__(self, parent, uuid)

	def set_motors_velocities(self, left, right):
		self.send(SET_MOTORS_VELOCITIES_OPCODE, FLOAT(left), FLOAT(right))

	def goto(self, x, y, theta, linear_velocity=200, angular_velocity=pi/4):
		self.send(GOTO_OPCODE, FLOAT(x), FLOAT(y), FLOAT(theta), FLOAT(linear_velocity), FLOAT(angular_velocity))

	def set_velocities(self, linear_velocity, angular_velocity):
		self.send(SET_VELOCITIES_OPCODE, FLOAT(linear_velocity), FLOAT(angular_velocity))

	def stop(self):
		self.set_motors_velocities(0, 0)

	def set_position(self, x, y, theta):
		self.send(SET_POSITION_OPCODE, FLOAT(x), FLOAT(y), FLOAT(theta))
	
	def reset(self):
		self.set_position(0, 0, 0)

	def get_position(self, timeout=1):
		output = self.execute(GET_POSITION_OPCODE, timeout=timeout)
		x, y, theta = output.read(FLOAT, FLOAT, FLOAT)
		return x, y, theta
	
	def get_velocities(self, timeout=1):
		output = self.execute(GET_VELOCITIES_OPCODE, timeout=timeout)
		linear_velocity, angular_velocity = output.read(FLOAT, FLOAT)
		return linear_velocity, angular_velocity

	def set_tunings(self, identifier, Kp, Kd, Ki):
		self.send(SET_PID_TUNINGS_OPCODE, BYTE(identifier), FLOAT(Kp), FLOAT(Kd), FLOAT(Ki))
	
	def set_linear_velocity_PID_tunings(self, Kp, Ki, Kd):
		self.set_tunings(LINEAR_VELOCITY_PID_IDENTIFIER, Kp, Ki, Kd)
	
	def set_angular_velocity_PID_tunings(self, Kp, Ki, Kd):
		self.set_tunings(ANGULAR_VELOCITY_PID_IDENTIFIER, Kp, Ki, Kd)

	def set_linear_position_PID_tunings(self, Kp, Ki, Kd):
		self.set_tunings(LINEAR_POSITION_PID_IDENTIFIER, Kp, Ki, Kd)
	
	def set_angular_position_PID_tunings(self, Kp, Ki, Kd):
		self.set_tunings(ANGULAR_POSITION_PID_IDENTIFIER, Kp, Ki, Kd)

	def get_tunings(self, identifier, timeout=1):
		output = self.execute(GET_PID_TUNINGS_OPCODE, BYTE(identifier), timeout=timeout)
		Kp, Kd, Ki = output.read(FLOAT, FLOAT, FLOAT)
		return Kp, Kd, Ki

	def get_linear_velocity_PID_tunings(self, timeout=1):
		self.get_tunings(LINEAR_VELOCITY_PID_IDENTIFIER, timeout=timeout)
	
	def get_angular_velocity_PID_tunings(self, timeout=1):
		self.get_tunings(ANGULAR_VELOCITY_PID_IDENTIFIER, timeout=timeout)

	def get_linear_position_PID_tunings(self, timeout=1):
		self.get_tunings(LINEAR_POSITION_PID_IDENTIFIER, timeout=timeout)
	
	def get_angular_position_PID_tunings(self, timeout=1):
		self.get_tunings(ANGULAR_POSITION_PID_IDENTIFIER, timeout=timeout)


class Gripper(Module):
	def __init__(self, parent, uuid = 'gripper'):
		Module.__init__(self, parent, uuid)

	def enable(self):
		self.send(SET_GRIPPER_ENABLED_OPCODE, BYTE(1))

	def disable(self):
		self.send(SET_GRIPPER_ENABLED_OPCODE, BYTE(0))

	def setposition(self, position):
		self.send(SET_GRIPPER_POSITION_OPCODE, FLOAT(position))
