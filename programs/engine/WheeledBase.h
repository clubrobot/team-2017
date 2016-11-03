#ifndef __WHEELEDBASE_H__
#define __WHEELEDBASE_H__

#include <Arduino.h>

#include "DCMotor.h"
#include "RotaryEncoder.h"

struct WheeledBase
{
	WheeledBase();

	DCMotor			leftMotor;
	DCMotor 		rightMotor;

	DCDriver		driver;

	RotaryEncoder 	leftEncoder;
	RotaryEncoder 	rightEncoder;

	const float 	leftWheelRadius;
	const float 	rightWheelRadius;
	const float 	axleTrack;
};

#endif // __WHEELEDBASE_H__
