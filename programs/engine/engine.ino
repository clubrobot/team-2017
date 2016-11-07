#include <Arduino.h>
#include <robotcom.h>

#include "WheeledBase.h"
#include "Odometry.h"
#include "Control.h"

// Opcodes declaration

#define SET_MOTOR_SPEED_OPCODE 0x02
#define GET_WHEEL_VALUE_OPCODE 0x03


// Load the different modules

WheeledBase	base; // See WheeledBase.cpp for initialization details

Odometry	odometry(base);
Control		control(base, odometry);

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
{/*
	Serial.begin(115200);/*
	RobotCom::init();
	RobotCom::addCommand(SET_MOTOR_SPEED_OPCODE, setMotorSpeedCommand);
	RobotCom::addCommand(GET_WHEEL_VALUE_OPCODE, getWheelValueCommand);*/

	control.setVelocitySetpoint(0);
	control.setOmegaSetpoint(0);
}

void loop()
{/*
	RobotCom::executeCommands();*/

	// Integrate odometry
	odometry.update();/*
	Serial.print(odometry.getState().x);
	Serial.print(" ");
	Serial.print(odometry.getState().y);
	Serial.print(" ");
	Serial.print(odometry.getState().theta);
	Serial.print(" ");
	Serial.print(odometry.getMovement().dx_dt);
	Serial.print(" ");
	Serial.print(odometry.getMovement().dy_dt);
	Serial.print(" ");
	Serial.print(odometry.getMovement().omega);
	Serial.println();*/

	// Integrate engineering control
	control.step();

	// Delay
	delayMicroseconds(5000);
}
