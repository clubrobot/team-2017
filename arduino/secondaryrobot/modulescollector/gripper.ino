#include <Arduino.h>

#include "../../common/SerialTalks.h"
#include "../../common/Gripper.h"

// Opcodes declaration

#define SET_GRIPPER_ENABLED  0x04
#define SET_GRIPPER_POSITION 0x05


// Load the gripper module

//             | pin | min | max | idle | hold | drop |
Gripper gripper(   5,    0,  164,   0.6,   0.2,   1.0 );

// Instructions

bool setGripperEnabledInstruction(Deserializer& input, Serializer& output)
{
	byte enabled;
	input >> enabled;
	if (enabled)
		gripper.enable();
	else
		gripper.disable();
}

bool setGripperPositionInstruction(Deserializer& input, Serializer& output)
{
	float position;
	input >> position;
	gripper.enable();
	gripper.setPosition(position);
	return false;
}

void setup()
{
	talks.begin(Serial);
	talks.attach(SET_GRIPPER_ENABLED , setGripperEnabledInstruction);
	talks.attach(SET_GRIPPER_POSITION, setGripperPositionInstruction);
}

void loop()
{
	talks.execute();
}
