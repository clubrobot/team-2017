#include "instructions.h"

#include "addresses.h"

#include "../../common/DCMotor.h"
#include "../../common/Codewheel.h"
#include "../../common/CodewheelsOdometry.h"
#include "../../common/DCMotorsWheeledBase.h"

// Global variables

extern DCMotor leftWheel;
extern DCMotor rightWheel;

extern Codewheel leftCodewheel;
extern Codewheel rightCodewheel;

extern DCMotorsWheeledBase base;

extern PID linearVelocityPID;
extern PID angularVelocityPID;

extern CodewheelsOdometry odometry;

// Instructions

bool SET_OPENLOOP_VELOCITIES(SerialTalks& inst, Deserializer& input, Serializer& output)
{
	float leftVelocity  = input.read<float>();
	float rightVelocity = input.read<float>();

	base.disable();
	leftWheel .setVelocity(leftVelocity);
	rightWheel.setVelocity(rightVelocity);

	return false;
}

bool SET_VELOCITIES(SerialTalks& inst, Deserializer& input, Serializer& output)
{
	float linearVelocity  = input.read<float>();
	float angularVelocity = input.read<float>();
	
	base.enable();
	base.setLinearVelocity (linearVelocity);
	base.setAngularVelocity(angularVelocity);

	return false;
}

bool GOTO(SerialTalks& inst, Deserializer& input, Serializer& output)
{
	float x               = input.read<float>();
	float y               = input.read<float>();
	float theta           = input.read<float>();
	float linearVelocity  = input.read<float>();
	float angularVelocity = input.read<float>();

	talks.err << "GOTO: not implemented yet\n";
	
	return false;
}

bool SET_POSITION(SerialTalks& inst, Deserializer& input, Serializer& output)
{
	float x     = input.read<float>();
	float y     = input.read<float>();
	float theta = input.read<float>();

	odometry.calibrateXAxis(x);
	odometry.calibrateYAxis(y);
	odometry.calibrateOrientation(theta);

	return false;
}

bool GET_POSITION(SerialTalks& inst, Deserializer& input, Serializer& output)
{
	const Position& position = odometry.getPosition();
	
	output.write<float>(position.x);
	output.write<float>(position.y);
	output.write<float>(position.theta);
	return true;
}

bool GET_VELOCITIES(SerialTalks& inst, Deserializer& input, Serializer& output)
{
	const float linearVelocity  = odometry.getLinearVelocity ();
	const float angularVelocity = odometry.getAngularVelocity();
	
	output.write<float>(linearVelocity);
	output.write<float>(angularVelocity);
	return true;
}

bool SET_PID_TUNINGS(SerialTalks& inst, Deserializer& input, Serializer& output)
{
	byte  id = input.read<byte>();
	float Kp = input.read<float>();
	float Ki = input.read<float>();
	float Kd = input.read<float>();

	switch (id)
	{
	case LINEAR_VELOCITY_PID_IDENTIFIER:
		linearVelocityPID.setTunings(Kp, Ki, Kd);
		linearVelocityPID.saveTunings(LINEAR_VELOCITY_PID_ADDRESS);
		break;
	case ANGULAR_VELOCITY_PID_IDENTIFIER:
		angularVelocityPID.setTunings(Kp, Ki, Kd);
		linearVelocityPID.saveTunings(ANGULAR_VELOCITY_PID_ADDRESS);
		break;
	default:
		talks.err << "SET_PID_TUNINGS: unknown PID controller identifier: " << id << "\n";
		break;
	}

	return false;
}

bool GET_PID_TUNINGS(SerialTalks& inst, Deserializer& input, Serializer& output)
{
	byte id = input.read<byte>();

	switch (id)
	{
	case LINEAR_VELOCITY_PID_IDENTIFIER:
		output.write<float>(linearVelocityPID.getKp());
		output.write<float>(linearVelocityPID.getKi());
		output.write<float>(linearVelocityPID.getKd());
		return true;
	case ANGULAR_VELOCITY_PID_IDENTIFIER:
		output.write<float>(angularVelocityPID.getKp());
		output.write<float>(angularVelocityPID.getKi());
		output.write<float>(angularVelocityPID.getKd());
		return true;
	default:
		talks.err << "GET_PID_TUNINGS: unknown PID controller identifier: " << id << "\n";
		return true;
	}
}

