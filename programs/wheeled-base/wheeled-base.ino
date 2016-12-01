#include <Arduino.h>
#include <SerialTalks.h>

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

bool setSpeedCommand(Deserializer& input, Serializer& output)
{
	char leftSpeed, rightSpeed; // in cm/s
	input >> leftSpeed >> rightSpeed;
	base.leftMotor .setSpeed((float)leftSpeed  * 10);
	base.rightMotor.setSpeed((float)rightSpeed * 10);
	return false;
}

bool getStateCommand(Deserializer& input, Serializer& output)
{
	const State& s = odometry.getState();
	output << float(s.x) << float(s.y) << float(s.theta);
	return true;
}

void setup()
{
	talks.begin(Serial);
	talks.attach(SET_SPEED_OPCODE, setSpeedCommand);
	talks.attach(GET_STATE_OPCODE, getStateCommand);

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
	talks.out << s.x << " " << s.y << " " << s.theta << " ";
	talks.out << m.velocity << " " << m.omega << "\n";//*/

	// Integrate engineering control
	//control.step();

	// Delay
	delayMicroseconds(5000);
}
