#include <Arduino.h>
#include <robotcom.h>

#include "WheeledBase.h"
#include "Odometry.h"
#include "Control.h"

// Opcodes declaration

#define SET_SPEED_OPCODE 0x02
#define GET_STATE_OPCODE 0x03


// Load the different modules

WheeledBase	base; // See WheeledBase.cpp for initialization details

Odometry	odometry(base);
Control		control(base, odometry);

// Commands

int setSpeedCommand(int argc, byte argv[], byte outv[])
{
	if (argc > 2)
	{
		// Parameters
		const signed char leftSpeed		= argv[1]; // in cm/s
		const signed char rightSpeed	= argv[2]; // in cm/s

		// Procedure
		base.leftMotor .setSpeed((float)leftSpeed  * 10);
		base.rightMotor.setSpeed((float)rightSpeed * 10);
	}
	else
	{
		base.leftMotor .setSpeed(0);
		base.rightMotor.setSpeed(0);
	}
	return 0;
}

int getStateCommand(int argc, byte argv[], byte outv[])
{
	// Outputs
	int outc = 1;
	float& x		= *((float*)(&outv[outc])); outc += sizeof(float);
	float& y		= *((float*)(&outv[outc])); outc += sizeof(float);
	float& theta	= *((float*)(&outv[outc])); outc += sizeof(float);

	// Function
	const State& s = odometry.getState();
	x		= s.x;
	y		= s.y;
	theta	= s.theta;//*/

	return outc;
}

void setup()
{/*
	Serial.begin(115200);//*/
	RobotCom::init();
	RobotCom::addCommand(SET_SPEED_OPCODE, setSpeedCommand);
	RobotCom::addCommand(GET_STATE_OPCODE, getStateCommand);//*/
	
	control.setVelocitySetpoint(0);
	control.setOmegaSetpoint(0);
}

void loop()
{//*
	RobotCom::executeCommands();//*/

	// Integrate odometry
	odometry.update();/*
	Serial.print(odometry.getState().x);
	Serial.print(" ");
	Serial.print(odometry.getState().y);
	Serial.print(" ");
	Serial.print(odometry.getState().theta);
	Serial.print(" ");
	Serial.print(odometry.getMovement().velocity);
	Serial.print(" ");
	Serial.print(odometry.getMovement().omega);
	Serial.println();//*/

	// Integrate engineering control
	//control.step();

	// Delay
	delayMicroseconds(5000);
}
