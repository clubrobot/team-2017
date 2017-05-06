#ifndef __INSTRUCTIONS_H__
#define __INSTRUCTIONS_H__

#include "../common/SerialTalks.h"

#define _SET_ROLLER_VELOCITY_OPCODE 			0x03
#define _SET_FIRING_HAMMER_VELOCITY_OPCODE		0x04
#define _SET_AX_POSITION_OPCODE					0X05
#define _GET_AX_POSITION_OPCODE					0X06
#define _GET_AX_TORQUE_OPCODE					0X07
#define _SET_AX_VELOCITY_MOVE_OPCODE			0X08

void SET_ROLLER_VELOCITY(SerialTalks &inst, Deserializer &input, Serializer &output);

void SET_FIRING_HAMMER_VELOCITY(SerialTalks &inst, Deserializer &input, Serializer &output);

void SET_AX_POSITION(SerialTalks &inst, Deserializer &input, Serializer &output);

void GET_AX_TORQUE(SerialTalks &inst, Deserializer &input, Serializer &output);

void SET_AX_VELOCITY_MOVE(SerialTalks &inst, Deserializer &input, Serializer &output);

#endif