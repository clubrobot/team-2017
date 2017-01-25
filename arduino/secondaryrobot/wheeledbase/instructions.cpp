#include "instructions.h"

#include "addresses.h"

#include "../../common/DCMotor.h"
#include "../../common/Codewheel.h"
#include "../../common/CodewheelsOdometry.h"
#include "../../common/DCMotorsWheeledBase.h"
#include "../../common/TrajectoryPlanner.h"

// Global variables

extern DCMotor leftWheel;
extern DCMotor rightWheel;

extern Codewheel leftCodewheel;
extern Codewheel rightCodewheel;

extern DCMotorsWheeledBase base;

#if CONTROL_IN_POSITION
extern PID linearPositionPID;
extern PID angularPositionPID;
#else
extern PID linearVelocityPID;
extern PID angularVelocityPID;
#endif // CONTROL_IN_POSITION

extern CodewheelsOdometry odometry;

extern TrajectoryPlanner trajectory;

// Instructions

bool SET_OPENLOOP_VELOCITIES(SerialTalks& inst, Deserializer& input, Serializer& output)
{
	float leftVelocity  = input.read<float>();
	float rightVelocity = input.read<float>();

	base.disable();
	trajectory.disable();
	leftWheel .setVelocity(leftVelocity);
	rightWheel.setVelocity(rightVelocity);

	return false;
}

bool SET_VELOCITIES(SerialTalks& inst, Deserializer& input, Serializer& output)
{
	float linearVelocity  = input.read<float>();
	float angularVelocity = input.read<float>();
	
	base.enable();
	trajectory.disable();
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

	trajectory.setMaximumVelocities(linearVelocity, angularVelocity);
	
	trajectory.reset();
	trajectory.addWaypoint(Position(x, y, theta));
	base.enable();
	trajectory.enable();

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

#if CONTROL_IN_POSITION
	base.disable();
	base.enable();
#endif

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
#if CONTROL_IN_POSITION
	case LINEAR_POSITION_PID_IDENTIFIER:
		linearPositionPID.setTunings(Kp, Ki, Kd);
		linearPositionPID.saveTunings(LINEAR_POSITION_PID_ADDRESS);
		break;
	case ANGULAR_POSITION_PID_IDENTIFIER:
		angularPositionPID.setTunings(Kp, Ki, Kd);
		angularPositionPID.saveTunings(ANGULAR_POSITION_PID_ADDRESS);
		break;
#else
	case LINEAR_VELOCITY_PID_IDENTIFIER:
		linearVelocityPID.setTunings(Kp, Ki, Kd);
		linearVelocityPID.saveTunings(LINEAR_VELOCITY_PID_ADDRESS);
		break;
	case ANGULAR_VELOCITY_PID_IDENTIFIER:
		angularVelocityPID.setTunings(Kp, Ki, Kd);
		angularVelocityPID.saveTunings(ANGULAR_VELOCITY_PID_ADDRESS);
		break;
#endif // CONTROL_IN_POSITION
	default:
		talks.err << "SET_PID_TUNINGS: unknown PID controller identifier: " << id << "\n";
	}

	return false;
}

bool GET_PID_TUNINGS(SerialTalks& inst, Deserializer& input, Serializer& output)
{
	byte id = input.read<byte>();
	float Kp, Ki, Kd;

	switch (id)
	{
#if CONTROL_IN_POSITION
	case LINEAR_POSITION_PID_IDENTIFIER:
		Kp = linearPositionPID.getKp();
		Ki = linearPositionPID.getKi();
		Kd = linearPositionPID.getKd();
		break;
	case ANGULAR_POSITION_PID_IDENTIFIER:
		Kp = angularPositionPID.getKp();
		Ki = angularPositionPID.getKi();
		Kd = angularPositionPID.getKd();
		break;
#else
	case LINEAR_VELOCITY_PID_IDENTIFIER:
		Kp = linearVelocityPID.getKp();
		Ki = linearVelocityPID.getKi();
		Kd = linearVelocityPID.getKd();
		break;
	case ANGULAR_VELOCITY_PID_IDENTIFIER:
		Kp = angularVelocityPID.getKp();
		Ki = angularVelocityPID.getKi();
		Kd = angularVelocityPID.getKd();
		break;
#endif // CONTROL_IN_POSITION
	default:
		talks.err << "GET_PID_TUNINGS: unknown PID controller identifier: " << id << "\n";
		return true;
	}

	output.write<float>(Kp);
	output.write<float>(Ki);
	output.write<float>(Kd);
	return true;
}

