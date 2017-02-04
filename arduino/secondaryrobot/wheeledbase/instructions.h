#ifndef __talksRUCTIONS_H__
#define __talksRUCTIONS_H__

#include "../../common/SerialTalks.h"

// Opcodes declaration

#define SET_OPENLOOP_VELOCITIES_OPCODE   0x04

#define SET_VELOCITIES_OPCODE    0x06

#define START_TRAJECTORY_OPCODE  0x07
#define TRAJECTORY_ENDED_OPCODE  0x08

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
#define LINEAR_POSITION_TO_VELOCITY_PID_IDENTIFIER  0x06
#define ANGULAR_POSITION_TO_VELOCITY_PID_IDENTIFIER 0X07


// Instructions prototypes

void SET_OPENLOOP_VELOCITIES(SerialTalks& talks, Deserializer& input, Serializer& output);

void SET_VELOCITIES(SerialTalks& talks, Deserializer& input, Serializer& output);

void START_TRAJECTORY(SerialTalks& talks, Deserializer& input, Serializer& output);

void TRAJECTORY_ENDED(SerialTalks& talks, Deserializer& input, Serializer& output);

void SET_POSITION(SerialTalks& talks, Deserializer& input, Serializer& output);

void GET_POSITION(SerialTalks& talks, Deserializer& input, Serializer& output);

void GET_VELOCITIES(SerialTalks& talks, Deserializer& input, Serializer& output);

void SET_PID_TUNINGS(SerialTalks& talks, Deserializer& input, Serializer& output);

void GET_PID_TUNINGS(SerialTalks& talks, Deserializer& input, Serializer& output);

#endif // __talksRUCTIONS_H__
