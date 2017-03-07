#include "TrajectoryPlanner.h"
#include <math.h>
#include "mathutils.h"
#include "SerialTalks.h"


float TrajectoryPlanner::getLinearVelocitySetpoint() const
{
	return m_linearVelocitySetpoint;
}

float TrajectoryPlanner::getAngularVelocitySetpoint() const
{
	return m_angularVelocitySetpoint;
}

bool TrajectoryPlanner::hasReachedItsTarget() const
{
	return m_targetReached;
}

bool TrajectoryPlanner::addWaypoint(const Position& waypoint)
{
	if (m_remainingWaypoints < TRAJECTORYPLANNER_MAX_WAYPOINTS)
	{
		m_waypoints[m_remainingWaypoints++] = waypoint;
		m_turnOnTheSpot = false;
		return true;
	}
	return false;
}

void TrajectoryPlanner::reset()
{
	m_remainingWaypoints = 0;
	m_turnOnTheSpot = false;
	m_targetReached = false;
}

void TrajectoryPlanner::setCartesianPositionInput(const Position& position)
{
	m_cartesianPositionInput = position;
}

void TrajectoryPlanner::setLinearVelocityTunings (float Kp, float max)
{
	m_linearVelocityKp = Kp;
	m_maxLinearVelocity = max;
}

void TrajectoryPlanner::setAngularVelocityTunings(float Kp, float max)
{
	m_angularVelocityKp = Kp;
	m_maxAngularVelocity = max;
}

void TrajectoryPlanner::setThresholdRadius(float radius)
{
	m_thresholdRadius = radius;
}

void TrajectoryPlanner::setThresholdPositions(float linearPosition, float angularPosition)
{
	m_thresholdLinearPosition  = linearPosition;
	m_thresholdAngularPosition = angularPosition;
}

void TrajectoryPlanner::process(float timestep)
{
	const Position& current = m_cartesianPositionInput;
	const Position& target  = m_waypoints[0];

	const float dx = target.x - current.x;
	const float dy = target.y - current.y;

	// Use the robot frame of reference 
	const float du =  cos(current.theta) * dx + sin(current.theta) * dy;
	const float dv = -sin(current.theta) * dx + cos(current.theta) * dy;
	const float theta = target.theta - current.theta;

	// Compute the oriented distance between the robot and its target
	float d = sqrt(du * du + dv * dv);
	float gamma = atan2(dv, du);

	// Are we under the threshold radius?
	m_turnOnTheSpot |= d < m_thresholdRadius * 2 * abs(sin(gamma - theta));
	
	// WIP
	m_linearVelocitySetpoint  = 0;
	m_angularVelocitySetpoint = 0;

	// Clamp velocities setpoints to their maximum values
	m_linearVelocitySetpoint  = saturate(m_linearVelocitySetpoint,  -m_maxLinearVelocity,  m_maxLinearVelocity);
	m_angularVelocitySetpoint = saturate(m_angularVelocitySetpoint, -m_maxAngularVelocity, m_maxAngularVelocity);

	// Has the robot reached its target?
	m_targetReached = d < m_thresholdLinearPosition;
	m_targetReached &= abs(inrange(theta, -M_PI, M_PI)) < m_thresholdAngularPosition;
}
