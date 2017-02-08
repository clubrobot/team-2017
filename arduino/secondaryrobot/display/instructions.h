#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "../../common/SerialTalks.h"
#include "configuration.h"

// Opcodes declaration

#define SET_MATRIX_MESSAGE_OPCODE 0x0D
#define SET_IPDISPLAY_MESSAGE_OPCODE 0x0D

// Instructions prototypes

void SET_MATRIX_MESSAGE(SerialTalks &talks, Deserializer &input, Serializer &output);
void SET_IPDISPLAY_MESSAGE(SerialTalks &talks, Deserializer &input, Serializer &output);

#endif