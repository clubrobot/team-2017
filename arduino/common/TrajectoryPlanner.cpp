#include "TrajectoryPlanner.h"
#include <math.h>
#include "mathutils.h"
#include "SerialTalks.h"


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

void TrajectoryPlanner::setMaximumAccelerations(float linearAcceleration, float angularAcceleration)
{
	m_maximumLinearAcceleration  = linearAcceleration;
	m_maximumAngularAcceleration = angularAcceleration;
}

void TrajectoryPlanner::setTimestep(float timestep)
{
	m_timestep = timestep;
}

void TrajectoryPlanner::enable()
{
	if (!m_enabled)
	{
		m_enabled = true;
		m_clock.restart();
	}
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
	if (m_enabled && m_clock.getElapsedTime() > m_timestep)
	{
		const float timestep = m_clock.restart();
		
		const Position& current = m_odometry->getPosition();
		const Position& target  = m_waypoints[0];

		const float dx = target.x - current.x;
		const float dy = target.y - current.y;

		float linearDelta  = sqrt(dx * dx + dy * dy);
		float angularDelta = inrange(atan2(dy, dx) - current.theta, -M_PI, M_PI);

		// Let the robot go backward if the angle is greater than 90°
		if (fabs(angularDelta) > M_PI / 2)
		{
			linearDelta  = -linearDelta;
			angularDelta = inrange(angularDelta + M_PI, -M_PI, M_PI);
		}

		// Set velocities setpoints
		float linearVelocitySetpoint  = saturate(linearDelta,  -m_maximumLinearVelocity,  m_maximumLinearVelocity);
		float angularVelocitySetpoint = saturate(angularDelta, -m_maximumAngularVelocity, m_maximumAngularVelocity);
		m_wheeledbase->setLinearVelocity (linearVelocitySetpoint);
		m_wheeledbase->setAngularVelocity(angularVelocitySetpoint);
	}
}
