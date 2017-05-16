#include "instructions.h"
#include "../common/AX12.h"
#include "../common/DCMotor.h"


#define CCWMAX 512
#define CWMAX 512


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
	float position = input.read<float>();
	int velocity = input.read<int>();
	servoax.moveSpeed(position, velocity);
}

void PING_AX(SerialTalks &inst, Deserializer &input, Serializer &output){
	output.write<int>(servoax.ping());
}

void SET_AX_HOLD(SerialTalks &inst, Deserializer &input, Serializer &output){
	servoax.hold(input.read<int>());
}

void GET_AX_POSITION(SerialTalks &inst, Deserializer &input, Serializer &output){
	output.write<float>(servoax.readPosition());
}

void CHECK_AX(SerialTalks &inst, Deserializer &input, Serializer &output){
	int result = servoax.moving();
	bool ok = true; 
		if(result == 1){
			ok = true;
		}
		else if(result == 0){
			int torque = servoax.readTorque();
			if(torque >= CCWMAX){
				ok = false;
			}
			else if(torque <= CWMAX){
				ok = false;
			}
		}
		if(!ok){
			servoax.hold(0);
		}
	output.write<bool>(ok);
}