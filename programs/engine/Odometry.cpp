#include "Odometry.h"
#include <math.h>

#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif


Odometry::Odometry(WheeledBase& base)
:	m_state(0, 0, 0)
,	m_movement(0, 0, 0)

,	m_base(base)
{
	// Nothing to do here...
}

const Odometry::State& Odometry::getState() const
{
	return m_state;
}

const Odometry::Movement& Odometry::getMovement() const
{
	return m_movement;
}

void Odometry::setState(float x, float y, float theta)
{
	setState(State(x, y, theta));
}

void Odometry::setState(const State& state)
{
	m_state = state;
}

void Odometry::setMovement(float dx, float dy, float omega)
{
	setMovement(Movement(dx, dy, omega));
}

void Odometry::setMovement(const Movement& movement)
{
	m_movement = movement;
}

void Odometry::update()
{
	const float dL = m_base.leftEncoder.getTraveledDistance();
	const float dR = m_base.rightEncoder.getTraveledDistance();
	const float dM = (dL + dR) / 2;

	m_state.x += dM * cos(m_state.theta);
	m_state.y += dM * sin(m_state.theta);
	m_state.theta += (dR - dL) / m_base.axleTrack;
}
