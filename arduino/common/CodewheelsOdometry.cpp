#include "CodewheelsOdometry.h"


void CodewheelsOdometry::setWheels(Codewheel& leftWheel, Codewheel& rightWheel)
{
	m_leftWheel  = &leftWheel;
	m_rightWheel = &rightWheel;
}

float CodewheelsOdometry::getLeftWheelTraveledDistance ()
{
	m_leftWheel->update();
	return m_leftWheel->restart();
}

float CodewheelsOdometry::getRightWheelTraveledDistance()
{
	m_rightWheel->update();
	return m_rightWheel->restart();
}
