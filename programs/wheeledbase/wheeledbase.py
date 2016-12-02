#!/usr/bin/env python3
#-*- coding: utf-8 -*-

from serialtalks import *

SET_SPEED_OPCODE = 0x02
GET_STATE_OPCODE = 0x03
SET_STATE_OPCODE = 0x04


class WheeledBase(SerialTalks):

	def __init__(self, port = '/dev/arduino/wheeledbase'):
		SerialTalks.__init__(self, port)

	def setspeed(self, left, right):
		self.send(SET_SPEED_OPCODE, FLOAT(left), FLOAT(right))

	def stop(self):
		self.setspeed(0, 0)

	def getstate(self, timeout = None):
		self.send(GET_STATE_OPCODE)
		output = self.poll(GET_STATE_OPCODE, timeout)
		x, y, theta = output.read(FLOAT, FLOAT, FLOAT)
		return x, y, theta
	
	def setstate(self, x, y, theta):
		self.send(SET_STATE_OPCODE, FLOAT(x), FLOAT(y), FLOAT(theta))
	
	def resetstate(self):
		self.setstate(0, 0, 0)
