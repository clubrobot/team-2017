#ifndef __INSTRUCTION_H__
#define __INSTRUCTION_H__

#include "../../common/SerialTalks.h"

// Opcodes declaration
#define GET_MESURE_OPCODE 0x02
#define MESURE 0x03


// Instructions prototypes
bool GET_MESURE(SerialTalks& inst, Deserializer& input, Serializer& output);
bool MESURE(SerialTalks& inst, Deserializer& input, Serializer& output);

#endif