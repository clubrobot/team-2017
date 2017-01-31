#include "TrajectoryPlanner.h"
#include <math.h>
#include "mathutils.h"
#include "SerialTalks.h"


float TrajectoryPlanner::getLinearPositionSetpoint() const
{
	return m_linearPositionSetpoint;
}

float TrajectoryPlanner::getAngularPositionSetpoint() const
{
	return m_angularPositionSetpoint;
}

bool TrajectoryPlanner::addWaypoint(const Position& waypoint)
{
	if (m_remainingWaypoints < TRAJECTORYPLANNER_MAX_WAYPOINTS)
	{
		m_waypoints[m_remainingWaypoints++] = waypoint;
		return true;
	}
	return false;
}

void TrajectoryPlanner::reset()
{
	m_remainingWaypoints = 0;
}

void TrajectoryPlanner::setCartesianPositionInput(const Position& position)
{
	m_cartesianPositionInput = position;
}

void TrajectoryPlanner::setThresholdRadius(float radius)
{
	m_thresholdRadius = radius;
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
	float linearDelta  = sqrt(du * du + dv * dv);
	float angularDelta = atan2(dv, du);

	// Are we under the threshold radius?
	bool underThresholdRadius = linearDelta < m_thresholdRadius * 2 * abs(sin(angularDelta - theta));

	// Compute the needed orientation to reach the target position with the right orientation
	float m_angularPositionSetpoint = (!underThresholdRadius) ?
		theta + 2 * (angularDelta - theta) :
		theta;
	m_angularPositionSetpoint = inrange(m_angularPositionSetpoint, -M_PI, M_PI);

	// Let the robot turn on the spot if it is not well oriented
	bool turnOnTheSpot = !underThresholdRadius && cos(m_angularPositionSetpoint) < 0;

	// Compute the circular arc distance to be traveled in order to reach the destination
	// The robot may move backward depending on which circular arc it is located
	if (!underThresholdRadius && !turnOnTheSpot)
	{
		float circularArcAngle = inrange(2 * (angularDelta - theta), -M_PI, M_PI);
		m_linearPositionSetpoint = linearDelta * circularArcAngle / (2 * sin(angularDelta - theta));
	}
	else if (turnOnTheSpot)
		m_linearPositionSetpoint = 0;
	else
		m_linearPositionSetpoint = du;
}
