#include "instructions.h"

#include "addresses.h"

#include "../../common/SerialTalks.h"
#include "../../common/DCMotor.h"
#include "../../common/VelocityController.h"
#include "../../common/PID.h"
#include "../../common/Codewheel.h"
#include "../../common/Odometry.h"
#include "../../common/TrajectoryPlanner.h"

// Global variables

extern DCMotorsDriver driver;

extern DCMotor leftWheel;
extern DCMotor rightWheel;

extern VelocityController velocityControl;

extern PID linVelPID;
extern PID angVelPID;

extern Codewheel leftCodewheel;
extern Codewheel rightCodewheel;

extern Odometry odometry;

extern TrajectoryPlanner trajectory;

// Instructions

void SET_OPENLOOP_VELOCITIES(SerialTalks& talks, Deserializer& input, Serializer& output)
{
	float leftWheelVel  = input.read<float>();
	float rightWheelVel = input.read<float>();

	velocityControl.disable();
	trajectory.disable();
	leftWheel .setVelocity(leftWheelVel);
	rightWheel.setVelocity(rightWheelVel);
}

void GET_CODEWHEELS_COUNTERS(SerialTalks& talks, Deserializer& input, Serializer& output)
{
	long leftCodewheelCounter  = leftCodewheel. getCounter();
	long rightCodewheelCounter = rightCodewheel.getCounter();

	output.write<long>(leftCodewheelCounter);
	output.write<long>(rightCodewheelCounter);
}

void SET_VELOCITIES(SerialTalks& talks, Deserializer& input, Serializer& output)
{
	float linVelSetpoint = input.read<float>();
	float angVelSetpoint = input.read<float>();
	trajectory.disable();
	velocityControl.enable();
	velocityControl.setSetpoints(linVelSetpoint, angVelSetpoint);
}

void START_TRAJECTORY(SerialTalks& talks, Deserializer& input, Serializer& output)
{
	float x     = input.read<float>();
	float y     = input.read<float>();
	float theta = input.read<float>();

	velocityControl.enable();
	trajectory.reset();
	trajectory.addWaypoint(Position(x, y, theta));
	trajectory.enable();
}

void TRAJECTORY_ENDED(SerialTalks& talks, Deserializer& input, Serializer& output)
{
	output.write<byte>(trajectory.hasReachedItsTarget());
}

void SET_POSITION(SerialTalks& talks, Deserializer& input, Serializer& output)
{
	float x     = input.read<float>();
	float y     = input.read<float>();
	float theta = input.read<float>();

	odometry.setPosition(x, y, theta);
}

void GET_POSITION(SerialTalks& talks, Deserializer& input, Serializer& output)
{
	const Position& pos = odometry.getPosition();
	
	output.write<float>(pos.x);
	output.write<float>(pos.y);
	output.write<float>(pos.theta);
}

void GET_VELOCITIES(SerialTalks& talks, Deserializer& input, Serializer& output)
{
	const float linVel = odometry.getLinVel();
	const float angVel = odometry.getAngVel();
	
	output.write<float>(linVel);
	output.write<float>(angVel);
}

void SET_PID_TUNINGS(SerialTalks& talks, Deserializer& input, Serializer& output)
{
	byte  id = input.read<byte>();
	float Kp = input.read<float>();
	float Ki = input.read<float>();
	float Kd = input.read<float>();
	
	switch (id)
	{
	case LINEAR_VELOCITY_PID_IDENTIFIER:
		linVelPID.setTunings(Kp, Ki, Kd);
		linVelPID.saveTunings(LINEAR_VELOCITY_PID_ADDRESS);
		break;
	case ANGULAR_VELOCITY_PID_IDENTIFIER:
		angVelPID.setTunings(Kp, Ki, Kd);
		angVelPID.saveTunings(ANGULAR_VELOCITY_PID_ADDRESS);
		break;
	}
}

void GET_PID_TUNINGS(SerialTalks& talks, Deserializer& input, Serializer& output)
{
	byte id = input.read<byte>();
	float Kp, Ki, Kd;

	switch (id)
	{
	case LINEAR_VELOCITY_PID_IDENTIFIER:
		Kp = linVelPID.getKp();
		Ki = linVelPID.getKi();
		Kd = linVelPID.getKd();
		break;
	case ANGULAR_VELOCITY_PID_IDENTIFIER:
		Kp = angVelPID.getKp();
		Ki = angVelPID.getKi();
		Kd = angVelPID.getKd();
		break;
	}

	output.write<float>(Kp);
	output.write<float>(Ki);
	output.write<float>(Kd);
}

