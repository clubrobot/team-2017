#!/usr/bin/env python3
#-*- coding: utf-8 -*-

from common.serialtalks   import *
from common.modulesrouter import Module

SET_GRIPPER_ENABLED_OPCODE  = 0x04
SET_GRIPPER_POSITION_OPCODE = 0x05


class Gripper(Module):
	def __init__(self, parent, uuid = 'gripper'):
		Module.__init__(self, parent, uuid)

	def enable(self):
		self.send(SET_GRIPPER_ENABLED_OPCODE, BYTE(1))

	def disable(self):
		self.send(SET_GRIPPER_ENABLED_OPCODE, BYTE(0))

	def setposition(self, position):
		self.send(SET_GRIPPER_POSITION_OPCODE, FLOAT(position))
