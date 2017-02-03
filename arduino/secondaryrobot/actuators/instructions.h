#ifndef __talksRUCTIONS_H__
#define __talksRUCTIONS_H__

#include "../../common/SerialTalks.h"

// Opcodes declaration

#define SET_DISPENSER_SETPOINT_OPCODE 0x0D


// Instructions prototypes

void SET_DISPENSER_SETPOINT(SerialTalks& talks, Deserializer& input, Serializer& output);

#endif // __talksRUCTIONS_H__
