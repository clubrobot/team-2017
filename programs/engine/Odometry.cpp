#include "Odometry.h"
#include <math.h>


Odometry::Odometry(WheeledBase& base)
:	m_state(0, 0, 0)
,	m_movement(0, 0, 0)

,	m_base(base)
{
	m_lastTime = micros();
}

const State& Odometry::getState() const
{
	return m_state;
}

const Movement& Odometry::getMovement() const
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

unsigned long Odometry::getElapsedTime()
{
	unsigned long currentTime = micros();
	unsigned long elapsedTime = currentTime - m_lastTime;
	m_lastTime = currentTime;
	return elapsedTime;
}

void Odometry::update()
{
	const float dt	= getElapsedTime();
	if (dt > 0)
	{
		const float dL	= m_base.leftEncoder.getTraveledDistance();
		const float dR	= m_base.rightEncoder.getTraveledDistance();
		const float dM	= (dL + dR) / 2;

		const float dx		= dM * cos(m_state.theta);
		const float dy		= dM * sin(m_state.theta);
		const float dtheta	= (dR - dL) / m_base.axleTrack;

		m_movement.dx_dt	= dx / dt;
		m_movement.dy_dt	= dy / dt;
		m_movement.omega	= dtheta / dt;

		m_state.x		+= dx;
		m_state.y		+= dy;
		m_state.theta	+= dtheta;
	}
}
