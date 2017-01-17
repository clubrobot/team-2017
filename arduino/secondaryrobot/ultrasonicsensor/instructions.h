#ifndef __INSTRUCTIONS_H__
#define __INSTRUCTIONS_H__

#include "../../common/SerialTalks.h"

// Opcodes declaration
#define GET_MESURE_OPCODE 0x06
#define MESURE_OPCODE     0x07


// Instructions prototypes
bool GET_MESURE(SerialTalks& inst, Deserializer& input, Serializer& output);
bool MESURE(SerialTalks& inst, Deserializer& input, Serializer& output);

#endif