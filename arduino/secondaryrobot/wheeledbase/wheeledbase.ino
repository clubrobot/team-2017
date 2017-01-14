#include <Arduino.h>

#include "../../common/SerialTalks.h"
#include "../../common/DCMotor.h"
#include "../../common/Codewheel.h"
#include "../../common/CodewheelsOdometry.h"
#include "../../common/DCMotorsWheeledBase.h"

// Opcodes declaration

#define SET_MOTORS_SPEEDS_OPCODE 0x04

#define MOVE_OPCODE              0x06
#define GOTO_OPCODE              0x07

#define SET_STATE_OPCODE         0x0A
#define GET_STATE_OPCODE         0x0B
#define GET_VELOCITIES_OPCODE    0x0C

#define SET_PID_TUNINGS_OPCODE   0x0E
#define GET_PID_TUNINGS_OPCODE   0x0F

// PID controllers identifiers

#define LINEAR_VELOCITY_PID_IDENTIFIER  0x02
#define LINEAR_VELOCITY_PID_ADDRESS     0x040

#define ANGULAR_VELOCITY_PID_IDENTIFIER 0x03
#define ANGULAR_VELOCITY_PID_ADDRESS    0x060

// Load the different modules

DCMotor leftWheel;
DCMotor rightWheel;

Codewheel leftCodewheel;
Codewheel rightCodewheel;

DCMotorsWheeledBase base;

PID linearVelocityPID (LINEAR_VELOCITY_PID_ADDRESS);
PID angularVelocityPID(ANGULAR_VELOCITY_PID_ADDRESS);

CodewheelsOdometry odometry;

// Instructions

bool setMotorsSpeedsInstruction(SerialTalks& inst, Deserializer& input, Serializer& output)
{
	float leftVelocity, rightVelocity;
	input >> leftVelocity >> rightVelocity;

	leftWheel .setVelocity(leftVelocity);
	rightWheel.setVelocity(rightVelocity);

	return false;
}

bool moveInstruction(SerialTalks& inst, Deserializer& input, Serializer& output)
{
	float linear, angular;
	input >> linear >> angular;

	base.setLinearVelocity (linear);
	base.setAngularVelocity(angular);

	return false;
}

bool gotoInstruction(SerialTalks& inst, Deserializer& input, Serializer& output)
{
	float x, y, theta, linear, angular;
	input >> x >> y >> theta >> linear >> angular;

	talks.err << "gotoInstruction: not implemented yet\n";
	
	return false;
}

bool setStateInstruction(SerialTalks& inst, Deserializer& input, Serializer& output)
{
	float x, y, theta;
	input >> x >> y >> theta;

	odometry.calibrateXAxis(x);
	odometry.calibrateYAxis(y);
	odometry.calibrateOrientation(theta);

	return false;
}

bool getStateInstruction(SerialTalks& inst, Deserializer& input, Serializer& output)
{
	const Position& p = odometry.getPosition();
	output << float(p.x) << float(p.y) << float(p.theta);
	return true;
}

bool getVelocitiesInstruction(SerialTalks& inst, Deserializer& input, Serializer& output)
{
	const float linearVelocity  = odometry.getLinearVelocity ();
	const float angularVelocity = odometry.getAngularVelocity();
	output << float(linearVelocity) << float(angularVelocity);
	return true;
}

bool setPIDTuningsInstruction(SerialTalks& inst, Deserializer& input, Serializer& output)
{
	byte id;
	float Kp, Ki, Kd;
	input >> id >> Kp >> Ki >> Kd;
	switch (id)
	{
	case LINEAR_VELOCITY_PID_IDENTIFIER:
		linearVelocityPID.setTunings(Kp, Ki, Kd);
		return false;
	case ANGULAR_VELOCITY_PID_IDENTIFIER:
		angularVelocityPID.setTunings(Kp, Ki, Kd);
		return false;
	default:
		talks.err << "setPIDTuningInstruction: unknown PID controller identifier: " << id << "\n";
		return false;
	}
}

bool getPIDTuningsInstruction(SerialTalks& inst, Deserializer& input, Serializer& output)
{
	byte id;
	input >> id;

	switch (id)
	{
	case LINEAR_VELOCITY_PID_IDENTIFIER:
		output << linearVelocityPID.getKp();
		output << linearVelocityPID.getKi();
		output << linearVelocityPID.getKd();
		return true;
	case ANGULAR_VELOCITY_PID_IDENTIFIER:
		output << angularVelocityPID.getKp();
		output << angularVelocityPID.getKi();
		output << angularVelocityPID.getKd();
		return true;
	default:
		talks.err << "getPIDTuningInstruction: unknown PID controller identifier: " << id << "\n";
		return true;
	}
}

void setup()
{
	talks.begin(Serial);
	talks.bind(SET_MOTORS_SPEEDS_OPCODE, setMotorsSpeedsInstruction);
	talks.bind(MOVE_OPCODE             , moveInstruction);
	talks.bind(GOTO_OPCODE             , gotoInstruction);
	talks.bind(SET_STATE_OPCODE        , setStateInstruction);
	talks.bind(GET_STATE_OPCODE        , getStateInstruction);
	talks.bind(GET_VELOCITIES_OPCODE   , getVelocitiesInstruction);
	talks.bind(SET_PID_TUNINGS_OPCODE  , setPIDTuningsInstruction);
	talks.bind(GET_PID_TUNINGS_OPCODE  , getPIDTuningsInstruction);

	TCCR2B = (TCCR2B & 0b11111000) | 1; // Set Timer2 frequency to 16MHz instead of 250kHz
}

void loop()
{
	talks.execute();

	// Integrate odometry
	odometry.update();

	// Delay
	delayMicroseconds(5000);
}
