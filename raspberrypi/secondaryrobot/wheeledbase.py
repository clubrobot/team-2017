#!/usr/bin/env python3
#-*- coding: utf-8 -*-

from math import pi

from serialtalks   import *
from modulesrouter import Module

SET_MOTORS_SPEEDS_OPCODE = 0x04

MOVE_OPCODE              = 0x06
GOTO_OPCODE              = 0x07

SET_STATE_OPCODE         = 0x0A
GET_STATE_OPCODE         = 0x0B
GET_VELOCITIES_OPCODE    = 0x0C

SET_PID_TUNINGS_OPCODE   = 0x0E
GET_PID_TUNINGS_OPCODE   = 0x0F

VELOCITY_CONTROLLER_IDENTIFIER = 0x02
OMEGA_CONTROLLER_IDENTIFIER    = 0x03


class WheeledBase(Module):
	def __init__(self, parent, uuid = 'wheeledbase'):
		Module.__init__(self, parent, uuid)

	def set_motors_speeds(self, left, right):
		self.send(SET_MOTORS_SPEEDS_OPCODE, FLOAT(left), FLOAT(right))

	def goto(self, x, y, theta, velocity = 100, omega = pi / 20):
		self.send(GOTO_OPCODE, FLOAT(x), FLOAT(y), FLOAT(theta), FLOAT(velocity), FLOAT(omega))

	def move(self, velocity, omega):
		self.send(MOVE_OPCODE, FLOAT(velocity), FLOAT(omega))

	def stop(self):
		self.set_motors_speeds(0, 0)

	def setstate(self, x, y, theta):
		self.send(SET_STATE_OPCODE, FLOAT(x), FLOAT(y), FLOAT(theta))
	
	def reset(self):
		self.setstate(0, 0, 0)

	def getstate(self, timeout = None):
		self.send(GET_STATE_OPCODE)
		output = self.poll(GET_STATE_OPCODE, timeout)
		x, y, theta = output.read(FLOAT, FLOAT, FLOAT)
		return x, y, theta
	
	def getvelocities(self, timeout = None):
		self.send(GET_VELOCITIES_OPCODE)
		output = self.poll(GET_VELOCITIES_OPCODE, timeout)
		velocity, omega = output.read(FLOAT, FLOAT)
		return velocity, omega

	def settunings(self, identifier, Kp, Kd, Ki):
		self.send(SET_PID_TUNINGS_OPCODE, BYTE(identifier), FLOAT(Kp), FLOAT(Kd), FLOAT(Ki))
	
	def set_velocity_tunings(self, Kp, Ki, Kd):
		self.settunings(VELOCITY_CONTROLLER_IDENTIFIER, Kp, Ki, Kd)
	
	def set_omega_tunings(self, Kp, Ki, Kd):
		self.settunings(OMEGA_CONTROLLER_IDENTIFIER, Kp, Ki, Kd)

	def gettunings(self, identifier, timeout = None):
		self.send(GET_PID_TUNINGS_OPCODE, BYTE(identifier))
		output = self.poll(GET_PID_TUNINGS_OPCODE, timeout)
		Kp, Kd, Ki = output.read(FLOAT, FLOAT, FLOAT)
		return Kp, Kd, Ki

	def get_velocity_tunings(self, timeout = None):
		return self.gettunings(VELOCITY_CONTROLLER_IDENTIFIER, timeout)

	def get_omega_tunings(self, timeout = None):
		return self.gettunings(OMEGA_CONTROLLER_IDENTIFIER, timeout)
