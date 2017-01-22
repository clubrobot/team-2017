#ifndef __INSTRUCTIONS_H__
#define __INSTRUCTIONS_H__

#include "../../common/SerialTalks.h"

// Opcodes declaration

#define SET_OPENLOOP_VELOCITIES_OPCODE   0x04

#define SET_VELOCITIES_OPCODE    0x06

#define GOTO_OPCODE              0x07

#define SET_POSITION_OPCODE      0x0A
#define GET_POSITION_OPCODE      0x0B
#define GET_VELOCITIES_OPCODE    0x0C

#define SET_PID_TUNINGS_OPCODE   0x0E
#define GET_PID_TUNINGS_OPCODE   0x0F

// PID controllers identifiers

#define LINEAR_VELOCITY_PID_IDENTIFIER  0x02
#define ANGULAR_VELOCITY_PID_IDENTIFIER 0x03
#define LINEAR_POSITION_PID_IDENTIFIER  0x04
#define ANGULAR_POSITION_PID_IDENTIFIER 0X05


// Instructions prototypes

bool SET_OPENLOOP_VELOCITIES(SerialTalks& inst, Deserializer& input, Serializer& output);

bool SET_VELOCITIES(SerialTalks& inst, Deserializer& input, Serializer& output);

bool GOTO(SerialTalks& inst, Deserializer& input, Serializer& output);

bool SET_POSITION(SerialTalks& inst, Deserializer& input, Serializer& output);

bool GET_POSITION(SerialTalks& inst, Deserializer& input, Serializer& output);

bool GET_VELOCITIES(SerialTalks& inst, Deserializer& input, Serializer& output);

bool SET_PID_TUNINGS(SerialTalks& inst, Deserializer& input, Serializer& output);

bool GET_PID_TUNINGS(SerialTalks& inst, Deserializer& input, Serializer& output);

#endif // __INSTRUCTIONS_H__
