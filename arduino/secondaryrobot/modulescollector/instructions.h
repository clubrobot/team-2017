#ifndef __INSTRUCTIONS_H__
#define __INSTRUCTIONS_H__

#include "../../common/SerialTalks.h"


#define _WRITE_DISPENSER_OPCODE     0x04
#define _WRITE_GRIP_OPCODE          0X05
#define _IS_UP_OPCODE               0x08
#define _IS_DOWN_OPCODE             0x09
#define _SET_MOTOR_VELOCITY_OPCODE  0x0C


void WRITE_GRIP(SerialTalks& inst, Deserializer& input, Serializer& output);

void WRITE_DISPENSER(SerialTalks& inst, Deserializer& input, Serializer& output);

void IS_UP(SerialTalks& inst, Deserializer& input, Serializer& output);

void IS_DOWN(SerialTalks& inst, Deserializer& input, Serializer& output);

void SET_MOTOR_VELOCITY(SerialTalks& inst, Deserializer& input, Serializer& output);


// position ouverte gripper = 142
// position fermée gripper = 4
 

#endif