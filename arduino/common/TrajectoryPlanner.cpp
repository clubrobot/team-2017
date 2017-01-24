#include "TrajectoryPlanner.h"


void TrajectoryPlanner::setOdometry(Odometry& odometry)
{
	m_odometry = &odometry;
}

void TrajectoryPlanner::setWheeledBase(WheeledBase& wheeledbase)
{
	m_wheeledbase = &wheeledbase;
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

void TrajectoryPlanner::setMaximumLinearVelocity(float linearVelocity)
{
	m_maximumLinearVelocity = linearVelocity;
}

void TrajectoryPlanner::setMaximumAngularVelocity(float angularVelocity)
{
	m_maximumAngularVelocity = angularVelocity;
}

void TrajectoryPlanner::enable()
{
	m_enabled = true;
}

void TrajectoryPlanner::disable()
{
	m_enabled = false;
}

void TrajectoryPlanner::reset()
{
	m_remainingWaypoints = 0;
}

void TrajectoryPlanner::update()
{

}
