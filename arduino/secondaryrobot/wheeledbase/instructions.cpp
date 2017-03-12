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
	trajectory.reset();
	trajectory.addWaypoint(odometry.getPosition());
	int numWaypoints = input.read<int>();
	for (int i = 0; i < numWaypoints; i++)
	{
		float x     = input.read<float>();
		float y     = input.read<float>();
		float theta = input.read<float>(); // Useless for now
		trajectory.addWaypoint(Position(x, y, theta));
	}
	velocityControl.enable();
	trajectory.enable();
}

void TRAJECTORY_ENDED(SerialTalks& talks, Deserializer& input, Serializer& output)
{
	output.write<byte>(false);
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

void SET_PARAMETER_VALUE(SerialTalks& talks, Deserializer& input, Serializer& output)
{
	byte  id = input.read<byte>();
	switch (id)
	{
	case LEFTWHEEL_RADIUS_ID:
		leftWheel.setWheelRadius(input.read<float>());
		leftWheel.save(LEFTWHEEL_ADDRESS);
		break;
	case LEFTWHEEL_CONSTANT_ID:
		leftWheel.setConstant(input.read<float>());
		leftWheel.save(LEFTWHEEL_ADDRESS);
		break;
	
	case RIGHTWHEEL_RADIUS_ID:
		rightWheel.setWheelRadius(input.read<float>());
		rightWheel.save(RIGHTWHEEL_ADDRESS);
		break;
	case RIGHTWHEEL_CONSTANT_ID:
		rightWheel.setConstant(input.read<float>());
		rightWheel.save(RIGHTWHEEL_ADDRESS);
		break;

	case LEFTCODEWHEEL_RADIUS_ID:
		leftCodewheel.setWheelRadius(input.read<float>());
		leftCodewheel.save(LEFTCODEWHEEL_ADDRESS);
		break;
	case LEFTCODEWHEEL_COUNTSPERREV_ID:
		leftCodewheel.setCountsPerRev(input.read<long>());
		leftCodewheel.save(LEFTCODEWHEEL_ADDRESS);
		break;
	
	case RIGHTCODEWHEEL_RADIUS_ID:
		rightCodewheel.setWheelRadius(input.read<float>());
		rightCodewheel.save(RIGHTCODEWHEEL_ADDRESS);
		break;
	case RIGHTCODEWHEEL_COUNTSPERREV_ID:
		rightCodewheel.setCountsPerRev(input.read<long>());
		rightCodewheel.save(RIGHTCODEWHEEL_ADDRESS);
		break;
	
	case ODOMETRY_AXLETRACK_ID:
		odometry.setAxleTrack(input.read<float>());
		odometry.save(ODOMETRY_ADDRESS);
		break;
	
	case VELOCITYCONTROL_AXLETRACK_ID:
		velocityControl.setAxleTrack(input.read<float>());
		velocityControl.save(VELOCITYCONTROL_ADDRESS);
		break;
	case VELOCITYCONTROL_MAXLINACC_ID:
		velocityControl.setMaxAcc(input.read<float>(), velocityControl.getMaxAngAcc());
		velocityControl.save(VELOCITYCONTROL_ADDRESS);
		break;
	case VELOCITYCONTROL_MAXLINDEC_ID:
		velocityControl.setMaxDec(input.read<float>(), velocityControl.getMaxAngDec());
		velocityControl.save(VELOCITYCONTROL_ADDRESS);
		break;
	case VELOCITYCONTROL_MAXANGACC_ID:
		velocityControl.setMaxAcc(velocityControl.getMaxLinAcc(), input.read<float>());
		velocityControl.save(VELOCITYCONTROL_ADDRESS);
		break;
	case VELOCITYCONTROL_MAXANGDEC_ID:
		velocityControl.setMaxDec(velocityControl.getMaxLinDec(), input.read<float>());
		velocityControl.save(VELOCITYCONTROL_ADDRESS);
		break;
	
	case LINVELPID_KP_ID:
		linVelPID.setTunings(input.read<float>(), linVelPID.getKi(), linVelPID.getKd());
		linVelPID.save(LINVELPID_ADDRESS);
		break;
	case LINVELPID_KI_ID:
		linVelPID.setTunings(linVelPID.getKp(), input.read<float>(), linVelPID.getKd());
		linVelPID.save(LINVELPID_ADDRESS);
		break;
	case LINVELPID_KD_ID:
		linVelPID.setTunings(linVelPID.getKp(), linVelPID.getKi(), input.read<float>());
		linVelPID.save(LINVELPID_ADDRESS);
		break;
	case LINVELPID_MINOUTPUT_ID:
		linVelPID.setOutputLimits(input.read<float>(), linVelPID.getMaxOutput());
		linVelPID.save(LINVELPID_ADDRESS);
		break;
	case LINVELPID_MAXOUTPUT_ID:
		linVelPID.setOutputLimits(linVelPID.getMinOutput(), input.read<float>());
		linVelPID.save(LINVELPID_ADDRESS);
		break;
	
	case ANGVELPID_KP_ID:
		angVelPID.setTunings(input.read<float>(), angVelPID.getKi(), angVelPID.getKd());
		angVelPID.save(ANGVELPID_ADDRESS);
		break;
	case ANGVELPID_KI_ID:
		angVelPID.setTunings(angVelPID.getKd(), input.read<float>(), angVelPID.getKd());
		angVelPID.save(ANGVELPID_ADDRESS);
		break;
	case ANGVELPID_KD_ID:
		angVelPID.setTunings(angVelPID.getKd(), angVelPID.getKi(), input.read<float>());
		angVelPID.save(ANGVELPID_ADDRESS);
		break;
	case ANGVELPID_MINOUTPUT_ID:
		angVelPID.setOutputLimits(input.read<float>(), angVelPID.getMaxOutput());
		angVelPID.save(ANGVELPID_ADDRESS);
		break;
	case ANGVELPID_MAXOUTPUT_ID:
		angVelPID.setOutputLimits(angVelPID.getMinOutput(), input.read<float>());
		angVelPID.save(ANGVELPID_ADDRESS);
		break;
	
	case TRAJECTORY_LINVELKP_ID:
		trajectory.setVelTunings(input.read<float>(), trajectory.getAngVelKp());
		trajectory.save(TRAJECTORY_ADDRESS);
		break;
	case TRAJECTORY_ANGVELKP_ID:
		trajectory.setVelTunings(trajectory.getLinVelKp(), input.read<float>());
		trajectory.save(TRAJECTORY_ADDRESS);
		break;
	case TRAJECTORY_LINVELMAX_ID:
		trajectory.setVelLimits(input.read<float>(), trajectory.getAngVelMax());
		trajectory.save(TRAJECTORY_ADDRESS);
		break;
	case TRAJECTORY_ANGVELMAX_ID:
		trajectory.setVelLimits(trajectory.getLinVelMax(), input.read<float>());
		trajectory.save(TRAJECTORY_ADDRESS);
		break;
	case TRAJECTORY_LOOKAHED_ID:
		trajectory.setLookAhead(input.read<float>());
		trajectory.save(TRAJECTORY_ADDRESS);
		break;
	}
}

void GET_PARAMETER_VALUE(SerialTalks& talks, Deserializer& input, Serializer& output)
{
	byte id = input.read<byte>();
	switch (id)
	{
	case LEFTWHEEL_RADIUS_ID:
		output.write<float>(leftWheel.getWheelRadius());
		break;
	case LEFTWHEEL_CONSTANT_ID:
		output.write<float>(leftWheel.getConstant());
		break;
	
	case RIGHTWHEEL_RADIUS_ID:
		output.write<float>(rightWheel.getWheelRadius());
		break;
	case RIGHTWHEEL_CONSTANT_ID:
		output.write<float>(rightWheel.getConstant());
		break;

	case LEFTCODEWHEEL_RADIUS_ID:
		output.write<float>(leftCodewheel.getWheelRadius());
		break;
	case LEFTCODEWHEEL_COUNTSPERREV_ID:
		output.write<long>(leftCodewheel.getCountsPerRev());
		break;
	
	case RIGHTCODEWHEEL_RADIUS_ID:
		output.write<float>(rightCodewheel.getWheelRadius());
		break;
	case RIGHTCODEWHEEL_COUNTSPERREV_ID:
		output.write<long>(rightCodewheel.getCountsPerRev());
		break;
	
	case ODOMETRY_AXLETRACK_ID:
		output.write<float>(odometry.getAxleTrack());
		break;
	
	case VELOCITYCONTROL_AXLETRACK_ID:
		output.write<float>(velocityControl.getAxleTrack());
		break;
	case VELOCITYCONTROL_MAXLINACC_ID:
		output.write<float>(velocityControl.getMaxLinAcc());
		break;
	case VELOCITYCONTROL_MAXLINDEC_ID:
		output.write<float>(velocityControl.getMaxLinDec());
		break;
	case VELOCITYCONTROL_MAXANGACC_ID:
		output.write<float>(velocityControl.getMaxAngAcc());
		break;
	case VELOCITYCONTROL_MAXANGDEC_ID:
		output.write<float>(velocityControl.getMaxAngDec());
		break;
	
	case LINVELPID_KP_ID:
		output.write<float>(linVelPID.getKp());
		break;
	case LINVELPID_KI_ID:
		output.write<float>(linVelPID.getKi());
		break;
	case LINVELPID_KD_ID:
		output.write<float>(linVelPID.getKd());
		break;
	case LINVELPID_MINOUTPUT_ID:
		output.write<float>(linVelPID.getMinOutput());
		break;
	case LINVELPID_MAXOUTPUT_ID:
		output.write<float>(linVelPID.getMaxOutput());
		break;
	
	case ANGVELPID_KP_ID:
		output.write<float>(angVelPID.getKp());
		break;
	case ANGVELPID_KI_ID:
		output.write<float>(angVelPID.getKi());
		break;
	case ANGVELPID_KD_ID:
		output.write<float>(angVelPID.getKd());
		break;
	case ANGVELPID_MINOUTPUT_ID:
		output.write<float>(angVelPID.getMinOutput());
		break;
	case ANGVELPID_MAXOUTPUT_ID:
		output.write<float>(angVelPID.getMaxOutput());
		break;

	case TRAJECTORY_LINVELKP_ID:
		output.write<float>(trajectory.getLinVelKp());
		break;
	case TRAJECTORY_ANGVELKP_ID:
		output.write<float>(trajectory.getAngVelKp());
		break;
	case TRAJECTORY_LINVELMAX_ID:
		output.write<float>(trajectory.getLinVelMax());
		break;
	case TRAJECTORY_ANGVELMAX_ID:
		output.write<float>(trajectory.getAngVelMax());
		break;
	case TRAJECTORY_LOOKAHED_ID:
		output.write<float>(trajectory.getLookAhead());
		break;
	}
}

