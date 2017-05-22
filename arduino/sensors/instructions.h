#ifndef __INSTRUCTIONS_H__
#define __INSTRUCTIONS_H__

#include "../common/SerialTalks.h"
#include "PIN.h"


// Opcodes declaration
#define GET_MESURE_OPCODE			0x06
#define ACTIVATE_SENSORS_OPCODE 	0x07
#define DESACTIVATE_SENSORS_OPCODE  0x08


// Instructions prototypes
void GET_MESURE(SerialTalks& inst, Deserializer& input, Serializer& output);

void ACTIVATE_SENSORS(SerialTalks& inst, Deserializer& input, Serializer& output);

void DESACTIVATE_SENSORS(SerialTalks& inst, Deserializer& input, Serializer& output);

#endif