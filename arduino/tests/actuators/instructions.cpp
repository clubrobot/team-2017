#include <Arduino.h>
#include <Servo.h>
#include "instructions.h"
#include "../common/AX12.h"
#include "../common/DCMotor.h"

extern AX12 servoax;

extern DCMotor motor1;
extern DCMotor motor2;

extern Servo servo1;
extern Servo servo2;
extern Servo servo3;
extern Servo servo4;

EndStop highStop;
EndStop lowStop;

//MOTOR
void SET_MOTOR1_VELOCITY(SerialTalks &inst, Deserializer &input, Serializer &output){
	Motor1.setVelocity(input.read<float>());
}

void SET_MOTOR2_VELOCITY(SerialTalks &inst, Deserializer &input, Serializer &output){
	Motor2.setVelocity(input.read<float>());
}

//AX12
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

//Servo
void SET_SERVO(SerialTalks &inst, Deserializer &input, Serializer &output){
	switch(input.read<int>()){
		default: servo1.write(input.read<int>());
				 break;
		case 2:	servo2.write(input.read<int>());
				break;
		case 3:	servo3.write(input.read<int>());
				break;
		case 4:	servo4.write(input.read<int>());
				break;
	}
}

//EndStop
void IS_UP(SerialTalks &inst, Deserializer &input, Serializer &output){
    output.write<int>(highStop.getState());
}

void IS_DOWN(SerialTalks &inst, Deserializer &input, Serializer &output){
    output.write<int>(lowStop.getState());
}