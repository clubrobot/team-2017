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

void TrajectoryPlanner::setMaximumVelocities(float linearVelocity, float angularVelocity)
{
	m_maximumLinearVelocity  = linearVelocity;
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
{/*
	const float Kp_lin = 1, lin_max = m_targetVelocities.linear , lin_dz = 10;
	const float Kp_ang = 1, ang_max = m_targetVelocities.angular, ang_dz = M_PI / 6;

	State r = m_odometry.getState(); r.theta = inrange(r.theta, -M_PI, M_PI);
	State t = m_targetStance;

	const float dx = t.x - r.x;
	const float dy = t.y - r.y;

	float dist = sqrt(dx * dx + dy * dy);
	float angl = inrange(atan2(dy, dx) - r.theta, -M_PI, M_PI);

	// Let the robot go backward if the angle is greater than 90°
	if (abs(angl) > M_PI / 2)
	{
		dist = -dist;
		angl = inrange(angl + M_PI, -M_PI, M_PI);
	}

	// Get the right orientation if the robot is close enough to its destination
	if (abs(dist) < lin_dz)
	{
		angl = inrange(t.theta - r.theta, -M_PI, M_PI);
	}
	// Else if make the robot turn on the spot if it is not well oriented to reach its destination
	else if (abs(angl) > ang_dz)
	{
		dist = 0;
	}
	talks.out << millis() << "\t" << saturate(Kp_lin * dist, -lin_max, lin_max) << "\t" << saturate(Kp_ang * angl, -ang_max, ang_max) << "\n";
	linearVelocitySetpoint  = saturate(Kp_lin * dist, -lin_max, lin_max);
	angularVelocitySetpoint = saturate(Kp_ang * angl, -ang_max, ang_max);*/
}
