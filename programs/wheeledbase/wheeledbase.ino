#include <Arduino.h>
#include <SerialTalks.h>

#include "WheeledBase.h"
#include "Odometry.h"
#include "Control.h"

// Opcodes declaration

#define SET_SPEED_OPCODE 0x02
#define GET_STATE_OPCODE 0x03
#define SET_STATE_OPCODE 0x04


// Load the different modules

WheeledBase	base; // See WheeledBase.cpp for initialization details

Odometry	odometry(base);
Control		control(base, odometry);

// Commands

bool setSpeedCommand(Deserializer& input, Serializer& output)
{
	float leftSpeed, rightSpeed; // in mm/s
	input >> leftSpeed >> rightSpeed;
	base.leftMotor .setSpeed(leftSpeed);
	base.rightMotor.setSpeed(rightSpeed);
	control.disable();
	return false;
}

bool getStateCommand(Deserializer& input, Serializer& output)
{
	const State& s = odometry.getState();
	output << float(s.x) << float(s.y) << float(s.theta);
	return true;
}

bool setStateCommand(Deserializer& input, Serializer& output)
{
	float x, y, theta;
	input >> x >> y >> theta;
	odometry.setState(x, y, theta);
	return false;
}

void setup()
{
	talks.begin(Serial);
	talks.attach(SET_SPEED_OPCODE, setSpeedCommand);
	talks.attach(GET_STATE_OPCODE, getStateCommand);
	talks.attach(SET_STATE_OPCODE, setStateCommand);

	control.setVelocitySetpoint(0);
	control.setOmegaSetpoint(0);
}

void loop()
{
	talks.execute();

	// Integrate odometry
	odometry.update();//*
	State    s = odometry.getState();
	Movement m = odometry.getMovement();
	//talks.out << s.x << " " << s.y << " " << s.theta << " ";
	//talks.out << m.velocity << " " << m.omega << "\n";//*/

	// Integrate engineering control
	control.step();

	// Delay
	delayMicroseconds(5000);
}
