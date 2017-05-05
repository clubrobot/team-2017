#!/usr/bin/env python3
#-*- coding: utf-8 -*-

import time
import math

from serialtalks import BYTE, INT, LONG, FLOAT
from components import SerialTalksProxy

# Instructions

_WRITE_DISPENSER_OPCODE    	= 0x04
_WRITE_GRIP_OPCODE      	= 0X05
_OPEN_GRIP_OPCODE			= 0x06
_SET_GRIP_VELOCITY_OPCODE	= 0X07
_IS_UP_OPCODE               = 0x08
_IS_DOWN_OPCODE             = 0x09
_GET_MOTOR_VELOCITY_OPCODE	= 0x0B
_SET_MOTOR_VELOCITY_OPCODE  = 0x0C


class ModulesGripper(SerialTalksProxy):	
	def __init__(self, parent, uuid='modulescollector'):
		SerialTalksProxy.__init__(self, parent, uuid)
		self.high_open_angle = 142
		self.low_open_angle = 80
		self.close_angle = 5

	def set_position(self,a):
		self.send(_WRITE_GRIP_OPCODE, INT(a))
	
	def set_velocity(self, a):
		self.send(_SET_GRIP_VELOCITY_OPCODE, FLOAT(a))

	def open_up(self):
		self.send(_OPEN_GRIP_OPCODE, INT(self.high_open_angle))

	def open_low(self):
		self.set_position(self.low_open_angle)

	def close(self):
		self.set_position(self.close_angle)


class ModulesDispenser(SerialTalksProxy):
	def __init__(self, parent, uuid='modulescollector'):
		SerialTalksProxy.__init__(self, parent, uuid)
		self.open_dispenser_angle = 180
		self.close_dispenser_angle = 0
	
	def set_position(self,a, t): 
		self.send(_WRITE_DISPENSER_OPCODE, INT(a), FLOAT(t))

	def open(self):
		self.set_position(self.open_dispenser_angle, 1.5)

	def close(self):
		self.set_position(self.close_dispenser_angle, 1.5)
		

class ModulesElevator(SerialTalksProxy):
	def __init__(self, parent, uuid='modulescollector'):
		SerialTalksProxy.__init__(self, parent, uuid)
		self.climbing_velocity = -11.1 
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
		output = self.execute(_SET_MOTOR_VELOCITY_OPCODE, FLOAT(a))
		moving = output.read(BYTE)
		return bool(moving)
	
	def get_velocity(self):
		output = self.execute(_GET_MOTOR_VELOCITY_OPCODE)
		velo = output.read(FLOAT)
		return float(velo)

	def go_up(self):
		if not self.set_velocity(self.climbing_velocity):
			raise RuntimeError("gripper not closed")
		while not self.isup():
			time.sleep(0.1)

	def go_down(self):
		if not self.set_velocity(self.going_down_velocity):
			raise RuntimeError('gripper not closed')
		while not self.isdown():
			time.sleep(0.1)
