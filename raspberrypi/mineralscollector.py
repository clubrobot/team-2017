#!/usr/bin/env python3
#-*- coding: utf-8 -*-

import time
import math

from serialtalks import BYTE, INT, LONG, FLOAT
from components import SerialTalksProxy
from threading import RLock

# Instructions

_SET_ROLLER_VELOCITY_OPCODE 		=	0x03
_SET_FIRING_HAMMER_VELOCITY_OPCODE	=	0x04
_RETURN_TO_SAFE_POSITION_OPCODE		= 	0x0B
_SETUP_AX_OPCODE					=	0x0C
_SET_AX_POSITION_OPCODE				=	0X05
_GET_AX_POSITION_OPCODE				=	0X06
_GET_AX_TORQUE_OPCODE				=	0X07
_SET_AX_VELOCITY_MOVE_OPCODE		=	0X08
_PING_AX_OPCODE						=	0x09
_SET_AX_HOLD_OPCODE					=	0X0A
_GET_AX_VELOCITY_OPCODE				=	0x10
_GET_AX_MOVING_OPCODE				=	0x0D


AX12_SEND_INSTRUCTION_PACKET_OPCODE = 0x0E
AX12_RECEIVE_STATUS_PACKET_OPCODE   = 0x0F

class AX12(SerialTalksProxy):	
	def __init__(self, parent, uuid='mineralscollector'):
		SerialTalksProxy.__init__(self, parent, uuid)		
		self.closed_position = 300
		self.collecting_position = 70
		self.mid_position = 200
		self.ax_lock = RLock()
		self.thread_safe_execute(_SETUP_AX_OPCODE)

	def thread_safe_execute(self, *args, **kwargs):
		self.ax_lock.acquire()
		try: return self.execute(*args, **kwargs) 
		except: raise
		finally: self.ax_lock.release()

	def set_position(self, a):
		self.thread_safe_execute(_SET_AX_POSITION_OPCODE, FLOAT(a))

	def get_position(self):
		output = self.thread_safe_execute(_GET_AX_POSITION_OPCODE)
		pos = output.read(FLOAT)
		return float(pos)
	
	def get_torque(self):
		output = self.thread_safe_execute(_GET_AX_TORQUE_OPCODE)
		trq = output.read(INT)
		return int(trq)
	
	def set_position_velocity(self, p, v):
		self.send(_SET_AX_VELOCITY_MOVE_OPCODE, FLOAT(p), INT(v))
	
	def ping(self):
		output = self.thread_safe_execute(_PING_AX_OPCODE)
		ping = output.read(INT)
		return int(ping)
	
	def hold(self, i):
		self.thread_safe_execute(_SET_AX_HOLD_OPCODE, INT(i))

	def gather(self):
		self.set_position_velocity(self.collecting_position, 300)
	
	def close(self):
		self.set_position_velocity(self.closed_position, 300)

	def set_closed_position(self, a):
		self.closed_position = a
	
	def set_collecting_position(self, a):
		self.collecting_position = a

	def send_instruction_packet(self, packet):
		self.send(AX12_SEND_INSTRUCTION_PACKET_OPCODE, BYTE(len(packet)), *map(BYTE, packet))

	def receive_status_packet(self):
		output = self.execute(AX12_RECEIVE_STATUS_PACKET_OPCODE)
		packet = []
		for i in range(output.read(BYTE)):
			packet.append(output.read(BYTE))
		return packet

	def read_data(self, address, length=1):
		self.receive_status_packet() # Purge status packets
		content = [0x02, 0x04, 0x02, address, length]
		checksum = ~sum(content) & 0xFF
		packet = [0xFF, 0xFF] + content + [checksum]
		self.send_instruction_packet(packet)
		time.sleep(0.1)
		return self.receive_status_packet()[5:-1]

	def write_data(self, address, data):
		content = [0x02, 3 + len(data), 0x03, address] + data
		checksum = ~sum(content) & 0xFF
		packet = [0xFF, 0xFF] + content + [checksum]
		self.send_instruction_packet(packet)
	
	def get_velocity(self):
		output = self.thread_safe_execute(_GET_AX_VELOCITY_OPCODE)
		vel = output.read(INT)
		return int(vel)
	
	def is_Moving(self):
		output = self.thread_safe_execute(_GET_AX_MOVING_OPCODE)
		mov = output.read(INT)
		time.sleep(0.1)
		return int(mov)


class Hammer(SerialTalksProxy):
	def __init__(self, parent, uuid='mineralscollector'):
		SerialTalksProxy.__init__(self, parent, uuid)
		self.firing_velocity = 9	

	def set_velocity(self, a):
		self.send(_SET_FIRING_HAMMER_VELOCITY_OPCODE, FLOAT(a))

	def fire(self):
		self.set_velocity(self.firing_velocity)
	
	def stop(self):
		self.set_velocity(0)

	def move_to_safe_position(self):
		self.send(_RETURN_TO_SAFE_POSITION_OPCODE)

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
