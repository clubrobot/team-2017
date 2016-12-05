#include <Arduino.h>
#include <SerialTalks.h>

#include "WheeledBase.h"
#include "Odometry.h"
#include "Control.h"

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
#define ANGULAR_VELOCITY_PID_IDENTIFIER 0x03


// Load the different modules

WheeledBase	base; // See WheeledBase.cpp for initialization details

Odometry	odometry(base);
Control		control(base, odometry);

// Instructions

bool setMotorsSpeedsInstruction(Deserializer& input, Serializer& output)
{
	float leftSpeed, rightSpeed;
	input >> leftSpeed >> rightSpeed;

	control.disable();
	base.leftMotor .setSpeed(leftSpeed);
	base.rightMotor.setSpeed(rightSpeed);

	return false;
}

bool moveInstruction(Deserializer& input, Serializer& output)
{
	float linear, angular;
	input >> linear >> angular;

	control.enable();
	control.setLinearVelocity (linear);
	control.setAngularVelocity(angular);

	return false;
}

bool gotoInstruction(Deserializer& input, Serializer& output)
{
	float x, y, theta;
	input >> x >> y >> theta;

	talks.err << "gotoInstruction: not implemented yet\n";
	
	return false;
}

bool setStateInstruction(Deserializer& input, Serializer& output)
{
	float x, y, theta;
	input >> x >> y >> theta;
	odometry.setState(x, y, theta);
	return false;
}

bool getStateInstruction(Deserializer& input, Serializer& output)
{
	const State& s = odometry.getState();
	output << float(s.x) << float(s.y) << float(s.theta);
	return true;
}

bool getVelocitiesInstruction(Deserializer& input, Serializer& output)
{
	const Movement& m = odometry.getMovement();
	output << float(m.linear) << float(m.angular);
	return true;
}

bool setPIDTuningsInstruction(Deserializer& input, Serializer& output)
{
	byte id;
	float Kp, Ki, Kd;
	input >> id >> Kp >> Ki >> Kd;
	switch (id)
	{
	case LINEAR_VELOCITY_PID_IDENTIFIER:
		control.setLinearVelocityPIDTunings(Kp, Ki, Kd);
		return false;
	case ANGULAR_VELOCITY_PID_IDENTIFIER:
		control.setAngularVelocityPIDTunings(Kp, Ki, Kd);
		return false;
	default:
		talks.err << "setPIDTuningInstruction: unknown PID controller identifier: " << id << "\n";
		return false;
	}
}

bool getPIDTuningsInstruction(Deserializer& input, Serializer& output)
{
	byte id;
	input >> id;

	switch (id)
	{
	case LINEAR_VELOCITY_PID_IDENTIFIER:
		output << control.getLinearVelocityPID().getKp();
		output << control.getLinearVelocityPID().getKi();
		output << control.getLinearVelocityPID().getKd();
		return true;
	case ANGULAR_VELOCITY_PID_IDENTIFIER:
		output << control.getAngularVelocityPID().getKp();
		output << control.getAngularVelocityPID().getKi();
		output << control.getAngularVelocityPID().getKd();
		return true;
	default:
		talks.err << "getPIDTuningInstruction: unknown PID controller identifier: " << id << "\n";
		return true;
	}
}

void setup()
{
	talks.begin(Serial);
	talks.attach(SET_MOTORS_SPEEDS_OPCODE, setMotorsSpeedsInstruction);
	talks.attach(MOVE_OPCODE             , moveInstruction);
	talks.attach(GOTO_OPCODE             , gotoInstruction);
	talks.attach(SET_STATE_OPCODE        , setStateInstruction);
	talks.attach(GET_STATE_OPCODE        , getStateInstruction);
	talks.attach(GET_VELOCITIES_OPCODE   , getVelocitiesInstruction);
	talks.attach(SET_PID_TUNINGS_OPCODE  , setPIDTuningsInstruction);
	talks.attach(GET_PID_TUNINGS_OPCODE  , getPIDTuningsInstruction);
}

void loop()
{
	talks.execute();

	// Integrate odometry
	odometry.update();

	// Integrate engineering control
	control.step();

	// Delay
	delayMicroseconds(5000);
}
