#ifndef __talksRUCTIONS_H__
#define __talksRUCTIONS_H__

#include "../../common/SerialTalks.h"

// Opcodes declaration

#define SET_DISPENSER_SETPOINT_OPCODE 0x0D
#define SET_MOTOR1_VELOCITY_OPCODE 0x0E
#define SET_MOTOR2_VELOCITY_OPCODE 0x0F


// Instructions prototypes

void SET_DISPENSER_SETPOINT(SerialTalks& talks, Deserializer& input, Serializer& output);
void SET_MOTOR1_VELOCITY(SerialTalks& talks, Deserializer& input, Serializer& output);
void SET_MOTOR2_VELOCITY(SerialTalks& talks, Deserializer& input, Serializer& output);


#endif // __talksRUCTIONS_H__
