#include <Arduino.h>
#include <robotcom.h>

#include "WheeledBase.h"
#include "Odometry.h"

// Opcodes declaration

#define SET_MOTOR_SPEED_OPCODE 0x02
#define GET_WHEEL_VALUE_OPCODE 0x03

// Global variables

WheeledBase base; // See WheeledBase.cpp for initialization details

Odometry odometry(base);

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
		DCMotor& motor = (code == 0) ? base.leftMotor : base.rightMotor;
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
		RotaryEncoder& wheel = (code == 0) ? base.leftEncoder : base.rightEncoder;
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

	// Integrate odometry
	odometry.update();

	// Delay
	delayMicroseconds(5000);
}
