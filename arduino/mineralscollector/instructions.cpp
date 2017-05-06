#include "instructions.h"
#include "../common/AX12.h"
#include "../common/DCMotor.h"

extern AX12 servoax;

extern DCMotor rollerMotor;

extern DCMotor hammerMotor;

void SET_ROLLER_VELOCITY(SerialTalks &inst, Deserializer &input, Serializer &output){
	rollerMotor.setVelocity(input.read<float>());
}

void SET_FIRING_HAMMER_VELOCITY(SerialTalks &inst, Deserializer &input, Serializer &output){
	hammerMotor.setVelocity(input.read<float>());
}

void SET_AX_POSITION(SerialTalks &inst, Deserializer &input, Serializer &output){
	servoax.move(input.read<float>());
}

void GET_AX_TORQUE(SerialTalks &inst, Deserializer &input, Serializer &output){
	int torque = servoax.readTorque();
	output.write<int>(torque);
}

void SET_AX_VELOCITY_MOVE(SerialTalks &inst, Deserializer &input, Serializer &output){
	servoax.moveSpeed(input.read<float>(), input.read<int>());
}