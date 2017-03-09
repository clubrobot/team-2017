#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "../../common/SerialTalks.h"
#include "configuration.h"

// Opcodes declaration

#define SET_MATRIX_MESSAGE_OPCODE 0x0D
#define SET_IPDISPLAY_MESSAGE_OPCODE 0x0E

#ifdef INIT_EEPROM
	#define INIT_EEPROM_DATA_OPCODE 0x0A				
#endif

// Instructions prototypes

void SET_MATRIX_MESSAGE(SerialTalks &talks, Deserializer &input, Serializer &output);
void SET_IPDISPLAY_MESSAGE(SerialTalks &talks, Deserializer &input, Serializer &output);
#ifdef INIT_EEPROM
	void INIT_EEPROM_DATA(SerialTalks &talks, Deserializer &input, Serializer &output);				
#endif

#endif