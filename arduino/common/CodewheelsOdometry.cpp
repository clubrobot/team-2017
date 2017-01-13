#include "CodewheelsOdometry.h"


void setWheels(Codewheel& leftWheel, Codewheel& rightWheel)
{
	m_leftWheel  = &leftWheel;
	m_rightWheel = &rightWheel;
}

float getLeftWheelTraveledDistance ()
{
	return m_leftWheel->restart();
}

float getRightWheelTraveledDistance()
{
	return m_rightWheel->restart();
}
