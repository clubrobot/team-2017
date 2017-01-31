#ifndef __TRAJECTORYPLANNER_H__
#define __TRAJECTORYPLANNER_H__

#include "PeriodicProcess.h"
#include "Odometry.h"

#ifndef TRAJECTORYPLANNER_MAX_WAYPOINTS
#define TRAJECTORYPLANNER_MAX_WAYPOINTS 10
#endif


class TrajectoryPlanner : public PeriodicProcess
{
public:

	float getLinearPositionSetpoint() const;
	float getAngularPositionSetpoint() const;

	bool addWaypoint(const Position& waypoint);
	void reset();
	
	void setCartesianPositionInput(const Position& position);

	void setThresholdRadius(float radius);
	
private:

	virtual void process(float timestep);

	Position m_cartesianPositionInput;
	float    m_linearPositionSetpoint;
	float    m_angularPositionSetpoint;

	Position m_waypoints[TRAJECTORYPLANNER_MAX_WAYPOINTS];
	int      m_remainingWaypoints;

	float m_thresholdRadius;
};

#endif // __TRAJECTORYPLANNER_H__
