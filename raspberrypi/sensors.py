#!/usr/bin/env python3
#-*- coding: utf-8 -*-

import time
import math

from serialtalks import BYTE, INT, LONG, FLOAT
from components import SerialTalksProxy

# Instructions

_GET_MESURE_SENSOR_OPCODE   = 0x06


class Sensors(SerialTalksProxy):
	def __init__(self, parent, uuid='sensors'):
		SerialTalksProxy.__init__(self, parent, uuid)

	def get_mesure(self,**kwargs):
		output = self.execute(_GET_MESURE_SENSOR_OPCODE, **kwargs)
		ar,av=output.read(INT,INT)
		return ar,av

