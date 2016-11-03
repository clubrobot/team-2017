#include "Odometry.h"
#include <math.h>

#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif


Odometry::Odometry(const RotaryEncoder& leftEncoder, const RotaryEncoder& rightEncoder, float axleTrack)
:	m_state(0, 0, 0)
,	m_leftEncoder(leftEncoder)
,	m_rightEncoder(rightEncoder)

,	m_axleTrack(axleTrack)
{
	// Nothing to do here...
}

const Odometry::State& Odometry::getState() const
{
	return m_state;
}

void Odometry::setState(float x, float y, float theta)
{
	setState(State(x, y, theta));
}

void Odometry::setState(const State& state)
{
	m_state = state;
}

void Odometry::integrate()
{
	const float dL = m_leftEncoder.getTraveledDistance();
	const float dR = m_rightEncoder.getTraveledDistance();
	const float dM = (dL + dR) / 2;

	m_state.x += dM * cos(m_state.theta);
	m_state.y += dM * sin(m_state.theta);
	m_state.theta += (dR - dL) / m_axleTrack;
}
