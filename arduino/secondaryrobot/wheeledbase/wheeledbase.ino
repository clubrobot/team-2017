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

DifferentialController positionController;
//DifferentialController velocityController;
VelocityController     velocityController;

PID linearVelocityController;
PID angularVelocityController;

Codewheel leftCodewheel;
Codewheel rightCodewheel;

Odometry odometry;

TrajectoryPlanner trajectory;

// Setup

void setup()
{
	// Communication
	Serial.begin(SERIALTALKS_BAUDRATE);
	talks.begin(Serial);
	talks.bind(SET_OPENLOOP_VELOCITIES_OPCODE, SET_OPENLOOP_VELOCITIES);
	talks.bind(SET_VELOCITIES_OPCODE, SET_VELOCITIES);
	talks.bind(START_TRAJECTORY_OPCODE, START_TRAJECTORY);
	talks.bind(TRAJECTORY_ENDED_OPCODE, TRAJECTORY_ENDED);
	talks.bind(SET_POSITION_OPCODE, SET_POSITION);
	talks.bind(GET_POSITION_OPCODE, GET_POSITION);
	talks.bind(GET_VELOCITIES_OPCODE, GET_VELOCITIES);
	talks.bind(SET_PID_TUNINGS_OPCODE, SET_PID_TUNINGS);
	talks.bind(GET_PID_TUNINGS_OPCODE, GET_PID_TUNINGS);

	// DC motors wheels
	driver.attach(DRIVER_RESET, DRIVER_FAULT);
	driver.reset();

	leftWheel.attach(LEFT_MOTOR_EN, LEFT_MOTOR_PWM, LEFT_MOTOR_DIR);
	leftWheel.setConstants(DCMOTORS_VELOCITY_CONSTANT, DCMOTORS_REDUCTION_RATIO);
	leftWheel.setSuppliedVoltage(DCMOTORS_SUPPLIED_VOLTAGE);
	leftWheel.setRadius(LEFT_WHEEL_RADIUS);

	rightWheel.attach(RIGHT_MOTOR_EN, RIGHT_MOTOR_PWM, RIGHT_MOTOR_DIR);
	rightWheel.setConstants(-DCMOTORS_VELOCITY_CONSTANT, DCMOTORS_REDUCTION_RATIO);
	rightWheel.setSuppliedVoltage(DCMOTORS_SUPPLIED_VOLTAGE);
	rightWheel.setRadius(RIGHT_WHEEL_RADIUS);

	// Engineering control
	velocityController.setAxleTrack(WHEELS_AXLE_TRACK);
	velocityController.setMaximumAccelerations (MAX_LINEAR_ACCELERATION,  MAX_ANGULAR_ACCELERATION);
	velocityController.setMaximumDeccelerations(MAX_LINEAR_DECCELERATION, MAX_ANGULAR_DECCELERATION);
	velocityController.setWheels(leftWheel, rightWheel);
	velocityController.setControllers(linearVelocityController, angularVelocityController);
	velocityController.disable();

	linearVelocityController .loadTunings(LINEAR_VELOCITY_PID_ADDRESS);
	angularVelocityController.loadTunings(ANGULAR_VELOCITY_PID_ADDRESS);
	const float maxLinearVelocity  = (leftWheel.getMaximumVelocity() + rightWheel.getMaximumVelocity()) / 2;
	const float maxAngularVelocity = (leftWheel.getMaximumVelocity() + rightWheel.getMaximumVelocity()) / WHEELS_AXLE_TRACK;
	linearVelocityController .setOutputLimits(-maxLinearVelocity,  maxLinearVelocity);
	angularVelocityController.setOutputLimits(-maxAngularVelocity, maxAngularVelocity);

	// Odometry
	leftCodewheel.attachCounter(QUAD_COUNTER_XY, QUAD_COUNTER_SEL1, QUAD_COUNTER_SEL2, QUAD_COUNTER_OE, QUAD_COUNTER_RST_Y);
	leftCodewheel.attachRegister(SHIFT_REG_DATA, SHIFT_REG_LATCH, SHIFT_REG_CLOCK);
	leftCodewheel.setAxis(Y);
	leftCodewheel.setCountsPerRevolution(-CODEWHEELS_COUNTS_PER_REVOLUTION); // negative -> backward
	leftCodewheel.setRadius(LEFT_CODEWHEEL_RADIUS);
	leftCodewheel.reset();

	rightCodewheel.attachCounter(QUAD_COUNTER_XY, QUAD_COUNTER_SEL1, QUAD_COUNTER_SEL2, QUAD_COUNTER_OE, QUAD_COUNTER_RST_X);
	rightCodewheel.attachRegister(SHIFT_REG_DATA, SHIFT_REG_LATCH, SHIFT_REG_CLOCK);
	rightCodewheel.setAxis(X);
	rightCodewheel.setCountsPerRevolution(CODEWHEELS_COUNTS_PER_REVOLUTION); // positive -> forward
	rightCodewheel.setRadius(RIGHT_CODEWHEEL_RADIUS);
	rightCodewheel.reset();

	odometry.setWheels(leftCodewheel, rightCodewheel);
	odometry.setAxleTrack(CODEWHEELS_AXLE_TRACK);
	odometry.setTimestep(ODOMETRY_TIMESTEP);
	odometry.enable();

	// Trajectories
	PID linearPositionToVelocityController;
	PID angularPositionToVelocityController;
	linearPositionToVelocityController .loadTunings(LINEAR_POSITION_TO_VELOCITY_PID_ADDRESS);
	angularPositionToVelocityController.loadTunings(ANGULAR_POSITION_TO_VELOCITY_PID_ADDRESS);
	trajectory.setLinearVelocityTunings (linearPositionToVelocityController .getKp(), MAX_LINEAR_VELOCITY);
	trajectory.setAngularVelocityTunings(angularPositionToVelocityController.getKp(), MAX_ANGULAR_VELOCITY);
	trajectory.setBezierCurveParameters(0.3, 0.4);
	trajectory.setThresholdRadius(10);
	trajectory.setThresholdPositions(MIN_LINEAR_POSITION, MIN_ANGULAR_POSITION);
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
		trajectory.setCartesianPositionInput(odometry.getPosition());
		velocityController.setInputs(odometry.getLinearVelocity(), odometry.getAngularVelocity());
	}

	// Compute trajectory
	if (trajectory.update())
	{
		float linearVelocitySetpoint  = trajectory.getLinearVelocitySetpoint();
		float angularVelocitySetpoint = trajectory.getAngularVelocitySetpoint();
		velocityController.setSetpoints(linearVelocitySetpoint, angularVelocitySetpoint);
	}

	// Integrate engineering control
	velocityController.update();
}
