#include "instructions.h"
#include "../common/AX12.h"
#include "../common/DCMotor.h"

#include "PIN.h"
#include "../common/SoftwareSerial.h"

extern AX12 servoax;

extern DCMotor rollerMotor;

extern DCMotor hammerMotor;

extern SoftwareSerial SoftSerial;

void SET_ROLLER_VELOCITY(SerialTalks &inst, Deserializer &input, Serializer &output){
	rollerMotor.setVelocity(input.read<float>());
}

void SET_FIRING_HAMMER_VELOCITY(SerialTalks &inst, Deserializer &input, Serializer &output){
	hammerMotor.setVelocity(input.read<float>());
}

void SET_AX_POSITION(SerialTalks &inst, Deserializer &input, Serializer &output){
	servoax.hold(1);
	servoax.setMaxTorqueRAM(1023);
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

void AX12_SEND_INSTRUCTION_PACKET(SerialTalks& talks, Deserializer& input, Serializer& output)
{
	int length = input.read<byte>();
	digitalWrite(DATA_CONTROL, 1); // switch to transmit mode
	for (int i = 0; i < length; i++)
	{
		byte toSend = input.read<byte>();
		SoftSerial.write(toSend);
	}
	delayMicroseconds(400);
	digitalWrite(DATA_CONTROL, 0); // switch to receive mode
}

void AX12_RECEIVE_STATUS_PACKET(SerialTalks& talks, Deserializer& input, Serializer& output)
{
	int length = SoftSerial.available();
	output.write<byte>(length);
	for (int i = 0; i < length; i++)
	{
		byte received = SoftSerial.read();
		output.write<byte>(received);
	}
}
