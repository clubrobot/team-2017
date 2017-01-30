#include "Odometry.h"
#include "SerialTalks.h"
#include <math.h>


const Position&	Odometry::getPosition() const
{
	return m_position;
}

float Odometry::getTraveledDistance() const
{
	return m_traveledDistance;
}

float Odometry::getLinearVelocity() const
{
	return m_linearVelocity;
}

float Odometry::getAngularVelocity() const
{
	return m_angularVelocity;
}

void Odometry::calibrateXAxis(float x)
{
	m_position.x = x;
}

void Odometry::calibrateYAxis(float y)
{
	m_position.y = y;
}

void Odometry::calibrateOrientation(float theta)
{
	m_position.theta = theta;
}

void Odometry::setAxleTrack(float axleTrack)
{
	m_axleTrack = axleTrack;
}

void Odometry::setWheels(AbstractCodewheel& leftWheel, AbstractCodewheel& rightWheel)
{
	m_leftWheel  = &leftWheel;
	m_rightWheel = &rightWheel;
}

void Odometry::setTimestep(float timestep)
{
	m_timestep = timestep;
}

void Odometry::update()
{
	if (m_clock.getElapsedTime() > m_timestep)
	{
		const float dt = m_clock.restart();
		
		const float dL = m_leftWheel ->restart();
		const float dR = m_rightWheel->restart();
		const float dM = (dL + dR) / 2;

		const float dtheta = (dR - dL) / m_axleTrack;
		const float mtheta = m_position.theta + dtheta / 2;
		const float dx     = dM * cos(mtheta);
		const float dy     = dM * sin(mtheta);

		m_position.x       += dx;
		m_position.y       += dy;
		m_position.theta   += dtheta;
		m_traveledDistance += dM;

		m_linearVelocity  = dM / dt;
		m_angularVelocity = dtheta / dt;
	}
}
