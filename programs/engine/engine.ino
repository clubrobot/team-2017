#include <Arduino.h>
#include <robotcom.h>

#include "DCMotor.h"
#include "RotaryEncoder.h"
#include "Odometry.h"

// Opcodes declaration

#define SET_MOTOR_SPEED_OPCODE 0x02
#define GET_WHEEL_VALUE_OPCODE 0x03

// Global variables
//						| EN | IN1 | IN2 |
DCMotor leftMotor		(  6,    5,    3 );
DCMotor rightMotor		(  9,   10,   11 );

//						| RESET | FAULT |
DCDriver driver			(    12,     A7 );

//						|             axis | XY | SEL1 | SEL2 | OE | RST |  Q | PL | CP | counts | radius |
RotaryEncoder leftWheel	( RotaryEncoder::Y,  A0,     7,     8,  A5,   A3,  13,   4,   2,   10000,   -22.7 );
RotaryEncoder rightWheel( RotaryEncoder::X,  A0,     7,     8,  A5,   A2,  13,   4,   2,   10000,   22.67 );

// Odometry

//					| left encoder | right encoder | axle track |
Odometry odometry	(    leftWheel,     rightWheel,         318 );
unsigned long time = 0;

// Commands

int setMotorSpeedCommand(int argc, byte argv[], byte outv[])
{
	if (argc > 3)
	{
		// Parameters
		const byte code	= argv[1]; // 0 = left motor, 1 = right motor
		const byte PWM	= argv[2]; // x = ratio * 255
		const byte dir	= argv[3]; // 0 = forward, 1 = backward

		// Procedure
		DCMotor& motor = (code == 0) ? leftMotor : rightMotor;
		if (dir == 0)
			motor.setSpeed(+float(PWM) / 255);
		else
			motor.setSpeed(-float(PWM) / 255);
	}
	return 0;
}

int getWheelValueCommand(int argc, byte argv[], byte outv[])
{
	if (argc > 1)
	{
		// Parameters
		const byte code	= argv[1]; // 0 = left wheel, 1 = right wheel

		// Outputs
		long& value = *((long*)(&outv[1]));
		int outc = sizeof(value);

		// Function
		RotaryEncoder& wheel = (code == 0) ? leftWheel : rightWheel;
		value = wheel.getCounter();

		return outc + 1;
	}
	return 1;
}

void setup()
{
	RobotCom::init();
	RobotCom::addCommand(SET_MOTOR_SPEED_OPCODE, setMotorSpeedCommand);
	RobotCom::addCommand(GET_WHEEL_VALUE_OPCODE, getWheelValueCommand);
}

void loop()
{
	RobotCom::executeCommands();

	// Compute elapsed time
	const unsigned long now = micros();
	const unsigned long dt = now - time;
	time = now;

	odometry.integrate();/*
	Serial.print("x: ");
	Serial.print(odometry.getState().x);
	Serial.print("\ty: ");
	Serial.print(odometry.getState().y);
	Serial.print("\ta: ");
	Serial.println(odometry.getState().theta);

	// Delay
	delayMicroseconds(5000);*/
}
