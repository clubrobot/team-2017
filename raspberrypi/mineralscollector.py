#!/usr/bin/env python3
#-*- coding: utf-8 -*-

import time
import math

from serialtalks import BYTE, INT, LONG, FLOAT
from components import SerialTalksProxy

# Instructions

_SET_ROLLER_VELOCITY_OPCODE 		=	0x03
_SET_FIRING_HAMMER_VELOCITY_OPCODE	=	0x04
_SET_AX_POSITION_OPCODE				=	0X05
_GET_AX_POSITION_OPCODE				=	0X06
_GET_AX_TORQUE_OPCODE				=	0X07
_SET_AX_VELOCITY_MOVE_OPCODE		=	0X08
_PING_AX_OPCODE						=	0x09
_SET_AX_HOLD_OPCODE					=	0X0A

class AX12(SerialTalksProxy):	
	def __init__(self, parent, uuid='mineralscollector'):
		SerialTalksProxy.__init__(self, parent, uuid)
		self.closed_position = 282
		self.collecting_position = 80

	def set_position(self, a):
		self.send(_SET_AX_POSITION_OPCODE, FLOAT(a))

	def get_position(self):
		output = self.execute(_GET_AX_POSITION_OPCODE)
		pos = output.read(FLOAT)
		return float(pos)
	
	def get_torque(self):
		output = self.execute(_GET_AX_TORQUE_OPCODE)
		trq = output.read(INT)
		return int(trq)
	
	def set_position_velocity(self, p, v):
		self.send(_SET_AX_VELOCITY_MOVE_OPCODE, FLOAT(p), INT(v))
	
	def ping(self):
		output = self.execute(_PING_AX_OPCODE)
		ping = output.read(INT)
		return int(ping)
	
	def hold(self, i):
		self.send(_SET_AX_HOLD_OPCODE, INT(i))

	def gather(self):
		self.set_position(self.collecting_position)
	
	def close(self):
		self.set_position(self.closed_position)

	def set_closed_position(self, a):
		self.closed_position = a
	
	def set_collecting_position(self, a):
		self.collecting_position = a

class Hammer(SerialTalksProxy):
	def __init__(self, parent, uuid='mineralscollector'):
		SerialTalksProxy.__init__(self, parent, uuid)
		self.firing_velocity = 6	

	def set_velocity(self, a):
		self.send(_SET_FIRING_HAMMER_VELOCITY_OPCODE, FLOAT(a))

	def fire(self):
		self.set_velocity(self.firing_velocity)
	
	def stop(self):
		self.set_velocity(0)

class Roller(SerialTalksProxy):
	def __init__(self, parent, uuid='mineralscollector'):
		SerialTalksProxy.__init__(self, parent, uuid)
		self.collecting_velocity = 8

	def set_velocity(self, a):
		self.send(_SET_ROLLER_VELOCITY_OPCODE, FLOAT(a))

	def gather(self):
		self.set_velocity(self.collecting_velocity)
	
	def stop(self):
		self.set_velocity(0)
