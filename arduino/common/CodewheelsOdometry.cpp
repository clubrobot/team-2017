#include "CodewheelsOdometry.h"


void CodewheelsOdometry::setWheels(Codewheel& leftWheel, Codewheel& rightWheel)
{
	m_leftWheel  = &leftWheel;
	m_rightWheel = &rightWheel;
}

float CodewheelsOdometry::getLeftWheelTraveledDistance ()
{
	return m_leftWheel->restart();
}

float CodewheelsOdometry::getRightWheelTraveledDistance()
{
	return m_rightWheel->restart();
}
