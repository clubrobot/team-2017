#ifndef __TRAJECTORYPLANNER_H__
#define __TRAJECTORYPLANNER_H__

#include "Odometry.h"
#include "WheeledBase.h"

#ifndef TRAJECTORYPLANNER_MAX_WAYPOINTS
#define TRAJECTORYPLANNER_MAX_WAYPOINTS 10
#endif


class TrajectoryPlanner
{
public:

	void setOdometry(Odometry& odometry);
	void setWheeledBase(WheeledBase& wheeledbase);

	bool addWaypoint(const Position& waypoint);

	void setMaximumVelocities(float linearVelocity, float angularVelocity);

	void enable();
	void disable();
	void reset();

	void update();

private:

	bool m_enabled;

	Odometry   * m_odometry;
	WheeledBase* m_wheeledbase;

	Position m_waypoints[TRAJECTORYPLANNER_MAX_WAYPOINTS];
	int      m_remainingWaypoints;

	float m_maximumLinearVelocity;
	float m_maximumAngularVelocity;
};

#endif // __TRAJECTORYPLANNER_H__
