#ifndef __INSTRUCTIONS_H__
#define __INSTRUCTIONS_H__

#include "../common/SerialTalks.h"
#include "PIN.h"

// Opcodes declaration
#define GET_MESURE_OPCODE 0x06


// Instructions prototypes
void GET_MESURE(SerialTalks& inst, Deserializer& input, Serializer& output);

#endif