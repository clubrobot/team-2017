#include <Arduino.h>

#include "PIN.h"
#include "constants.h"
#include "instructions.h"
#include "addresses.h"

#include "../../common/SerialTalks.h"
#include "../../common/DCMotor.h"
#include "../../common/Codewheel.h"
#include "../../common/CodewheelsOdometry.h"
#include "../../common/DCMotorsWheeledBase.h"

// Load the different modules

DCDriver driver;

DCMotor leftWheel;
DCMotor rightWheel;

Codewheel leftCodewheel;
Codewheel rightCodewheel;

DCMotorsWheeledBase base;

#if CONTROL_IN_POSITION
PID linearPositionPID;
PID angularPositionPID;
#else
PID linearVelocityPID;
PID angularVelocityPID;
#endif // CONTROL_IN_POSITION

CodewheelsOdometry odometry;

// Setup

void setup()
{
	talks.begin(Serial);
	talks.bind(SET_OPENLOOP_VELOCITIES_OPCODE, SET_OPENLOOP_VELOCITIES);
	talks.bind(SET_VELOCITIES_OPCODE, SET_VELOCITIES);
	talks.bind(GOTO_OPCODE, GOTO);
	talks.bind(SET_POSITION_OPCODE, SET_POSITION);
	talks.bind(GET_POSITION_OPCODE, GET_POSITION);
	talks.bind(GET_VELOCITIES_OPCODE, GET_VELOCITIES);
	talks.bind(SET_PID_TUNINGS_OPCODE, SET_PID_TUNINGS);
	talks.bind(GET_PID_TUNINGS_OPCODE, GET_PID_TUNINGS);

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

	base.setWheels(leftWheel, rightWheel);
	base.setOdometry(odometry);
#if CONTROL_IN_POSITION
	base.setPIDControllers(linearPositionPID, angularPositionPID);
#else
	base.setPIDControllers(linearVelocityPID, angularVelocityPID);
#endif
	base.setAxleTrack(WHEELS_AXLE_TRACK);
	base.disable();

	odometry.setWheels(leftCodewheel, rightCodewheel);
	odometry.setAxleTrack(CODEWHEELS_AXLE_TRACK);
	odometry.setTimestep(ODOMETRY_TIMESTEP);

#if CONTROL_IN_POSITION
	linearPositionPID.loadTunings(LINEAR_POSITION_PID_ADDRESS);
	linearPositionPID.setTimestep(PID_CONTROLLERS_TIMESTEP);
	linearPositionPID.reset();
	
	angularPositionPID.loadTunings(ANGULAR_POSITION_PID_ADDRESS);
	angularPositionPID.setTimestep(PID_CONTROLLERS_TIMESTEP);
	angularPositionPID.reset();
#else
	linearVelocityPID.loadTunings(LINEAR_VELOCITY_PID_ADDRESS);
	linearVelocityPID.setTimestep(PID_CONTROLLERS_TIMESTEP);
	linearVelocityPID.reset();
	
	angularVelocityPID.loadTunings(ANGULAR_VELOCITY_PID_ADDRESS);
	angularVelocityPID.setTimestep(PID_CONTROLLERS_TIMESTEP);
	angularVelocityPID.reset();
#endif // CONTROL_IN_POSITION

	TCCR2B = (TCCR2B & 0b11111000) | 1; // Set Timer2 frequency to 16MHz instead of 250kHz
}

// Loop

void loop()
{	
	talks.execute();

	// Integrate odometry
	odometry.update();

	// Integrate engineering control
	base.update();
}
