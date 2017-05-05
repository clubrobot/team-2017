#!/usr/bin/env python3
#-*- coding: utf-8 -*-

import time
import math

from serialtalks import BYTE, INT, LONG, FLOAT, STRING
from components import SerialTalksProxy

# Instructions

SET_EEPROM_DEFAULT_MESSAGE_OPCODE   = 0x09
SET_SPEED_MATRIX_OPCODE             = 0x0A
SET_EEPROM_CHAR_LEDMATRIX_OPCODE    = 0x0B
SET_EEPROM_CHAR_IPDISPLAY_OPCODE    = 0x0C
SET_MATRIX_MESSAGE_OPCODE           = 0x0D
SET_IPDISPLAY_MESSAGE_OPCODE        = 0x0E

SLIDE_MODE          = 0
ANIMATION_MODE      = 1
RIGHT_ROTATION_MODE = 2
LEFT_ROTATION_MODE  = 3
UPSIDEDOWN_MODE     = 4


class LEDMatrix(SerialTalksProxy):
	
	def __init__(self, parent, matrix_id, uuid='display'):
		SerialTalksProxy.__init__(self, parent, uuid)
		self.matrix_id = matrix_id

	def set_message(self, message, mode, speed=None):
		if not len(message) < 20:
			raise ValueError('message length must be less than 20 characters')
		self.send(SET_MATRIX_MESSAGE_OPCODE, BYTE(self.matrix_id), BYTE(mode), STRING(message))
		if speed is not None:
			self.set_speed(speed)
	
	def set_speed(self, speed):
		# `speed` in milliseconds
		self.send(SET_SPEED_MATRIX_OPCODE, BYTE(self.matrix_id), INT(speed))

	def set_default_message(self, message, mode, speed):
		if not len(message) < 8:
			raise ValueError('default message length must be less than 8 characters')
		self.send(SET_EEPROM_DEFAULT_MESSAGE_OPCODE, BYTE(self.matrix_id), INT(speed), BYTE(mode), STRING(message)) 
	
	def set_char_pattern(self, char, pattern):
		# `pattern` must be a PIL image with black (LED on) and white (LED off) pixels
		lines = []
		for y in range(8):
			line = 0
			for x in range(pattern.width):
				digit = ~(pattern.getpixel((x, y)) // 255)
				line = (line << 1) | (digit & 1)
			line <<= 8 - pattern.width
			lines.append(BYTE(line))
		self.execute(SET_EEPROM_CHAR_LEDMATRIX_OPCODE, CHAR(ord(char)), *lines, BYTE(pattern.width))


class SevenSegments(SerialTalksProxy):
	
	def __init__(self, parent, uuid='display'):
		SerialTalksProxy.__init__(self, parent, uuid)

	def set_message(self, message):
		if len(message) > 12:
			raise ValueError('message length must 12 characters or less')
		self.send(SET_IPDISPLAY_MESSAGE_OPCODE, STRING(message))

	def set_char_pattern(self, char, pattern):
		segments = 0
		for i, (x, y) in enumerate([(1, 0), (2, 1), (2, 3), (1, 4), (0, 3), (0, 1), (1, 2)])
			digit = ~(pattern.getpixel((x, y)) // 255)
			segments |= (digit << i)
		self.execute(SET_EEPROM_CHAR_IPDISPLAY_OPCODE, CHAR(ord(char)), BYTE(segments))
