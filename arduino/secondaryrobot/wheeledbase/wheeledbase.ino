#include <Arduino.h>

#include "PIN.h"
#include "constants.h"
#include "instructions.h"
#include "addresses.h"

#include "../../common/SerialTalks.h"
#include "../../common/DCMotor.h"
#include "../../common/DifferentialController.h"
#include "../../common/VelocityController.h"
#include "../../common/PID.h"
#include "../../common/Codewheel.h"
#include "../../common/Odometry.h"
#include "../../common/TrajectoryPlanner.h"
#include "../../common/mathutils.h"

// Load the different modules

DCMotorsDriver driver;

DCMotor leftWheel;
DCMotor rightWheel;

Codewheel leftCodewheel;
Codewheel rightCodewheel;

Odometry odometry;

VelocityController velocityControl;

PID linVelPID;
PID angVelPID;

TrajectoryPlanner trajectory;

// Setup

void setup()
{
	// Communication
	Serial.begin(SERIALTALKS_BAUDRATE);
	talks.begin(Serial);
	talks.bind(SET_OPENLOOP_VELOCITIES_OPCODE, SET_OPENLOOP_VELOCITIES);
	talks.bind(GET_CODEWHEELS_COUNTERS_OPCODE, GET_CODEWHEELS_COUNTERS);
	talks.bind(SET_VELOCITIES_OPCODE, SET_VELOCITIES);
	talks.bind(START_TRAJECTORY_OPCODE, START_TRAJECTORY);
	talks.bind(TRAJECTORY_ENDED_OPCODE, TRAJECTORY_ENDED);
	talks.bind(SET_POSITION_OPCODE, SET_POSITION);
	talks.bind(GET_POSITION_OPCODE, GET_POSITION);
	talks.bind(GET_VELOCITIES_OPCODE, GET_VELOCITIES);
	talks.bind(SET_PARAMETER_VALUE_OPCODE, SET_PARAMETER_VALUE);
	talks.bind(GET_PARAMETER_VALUE_OPCODE, GET_PARAMETER_VALUE);

	// DC motors wheels
	driver.attach(DRIVER_RESET, DRIVER_FAULT);
	driver.reset();

	leftWheel .attach(LEFT_MOTOR_EN,  LEFT_MOTOR_PWM,  LEFT_MOTOR_DIR);
	rightWheel.attach(RIGHT_MOTOR_EN, RIGHT_MOTOR_PWM, RIGHT_MOTOR_DIR);
	leftWheel .load(LEFTWHEEL_ADDRESS);
	rightWheel.load(RIGHTWHEEL_ADDRESS);

	// Codewheels
	leftCodewheel .attachCounter(QUAD_COUNTER_XY, QUAD_COUNTER_Y_AXIS, QUAD_COUNTER_SEL1, QUAD_COUNTER_SEL2, QUAD_COUNTER_OE, QUAD_COUNTER_RST_Y);
	rightCodewheel.attachCounter(QUAD_COUNTER_XY, QUAD_COUNTER_X_AXIS, QUAD_COUNTER_SEL1, QUAD_COUNTER_SEL2, QUAD_COUNTER_OE, QUAD_COUNTER_RST_X);
	leftCodewheel .attachRegister(SHIFT_REG_DATA, SHIFT_REG_LATCH, SHIFT_REG_CLOCK);
	rightCodewheel.attachRegister(SHIFT_REG_DATA, SHIFT_REG_LATCH, SHIFT_REG_CLOCK);
	leftCodewheel .load(LEFTCODEWHEEL_ADDRESS);
	rightCodewheel.load(RIGHTCODEWHEEL_ADDRESS);
	leftCodewheel .reset();
	rightCodewheel.reset();

	// Odometry
	odometry.load(ODOMETRY_ADDRESS);
	odometry.setCodewheels(leftCodewheel, rightCodewheel);
	odometry.setTimestep(ODOMETRY_TIMESTEP);
	odometry.enable();

	// Engineering control
	velocityControl.load(VELOCITYCONTROL_ADDRESS);
	velocityControl.setWheels(leftWheel, rightWheel);
	velocityControl.setPID(linVelPID, angVelPID);
	velocityControl.disable();

	const float maxLinVel = (leftWheel.getMaxVelocity() + rightWheel.getMaxVelocity()) / 2;
	const float maxAngVel = (leftWheel.getMaxVelocity() + rightWheel.getMaxVelocity()) / WHEELS_AXLE_TRACK;
	linVelPID.load(LINVELPID_ADDRESS);
	angVelPID.load(ANGVELPID_ADDRESS);
	linVelPID.setOutputLimits(-maxLinVel, maxLinVel);
	angVelPID.setOutputLimits(-maxAngVel, maxAngVel);

	// Trajectories
	trajectory.load(TRAJECTORY_ADDRESS);
	trajectory.setTimestep(TRAJECTORY_TIMESTEP);
	trajectory.disable();

	// Miscellanous
	TCCR2B = (TCCR2B & 0b11111000) | 1; // Set Timer2 frequency to 16MHz instead of 250kHz
}

// Loop

void loop()
{	
	talks.execute();

	// Update odometry
	if (odometry.update())
	{
		trajectory.setPosInput(odometry.getPosition());
		velocityControl.setInputs(odometry.getLinVel(), odometry.getAngVel());
	}

	// Compute trajectory
	if (trajectory.update())
	{
		float linVelSetpoint = trajectory.getLinVelSetpoint();
		float angVelSetpoint = trajectory.getAngVelSetpoint();
		velocityControl.setSetpoints(linVelSetpoint, angVelSetpoint);
	}

	// Integrate engineering control
	velocityControl.update();
}
