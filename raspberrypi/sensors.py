#!/usr/bin/env python3
#-*- coding: utf-8 -*-

import time
import math

from serialtalks import BYTE, INT, LONG, FLOAT
from components import SerialTalksProxy

# Instructions

_GET_MESURE_SENSOR_OPCODE   = 0x06
ACTIVATE_SENSORS_OPCODE 	= 0x07
DESACTIVATE_SENSORS_OPCODE  = 0x08


class Sensors(SerialTalksProxy):
	def __init__(self, parent, uuid='sensors'):
		SerialTalksProxy.__init__(self, parent, uuid)

	def get_mesure(self,**kwargs):
		output = self.execute(_GET_MESURE_SENSOR_OPCODE, **kwargs)
		ar,av=output.read(INT,INT)
		return ar,av

	def activate(self):
		self.send(ACTIVATE_SENSORS_OPCODE, BYTE(1))

	def desactivate(self):
		self.send(DESACTIVATE_SENSORS_OPCODE, BYTE(0))
