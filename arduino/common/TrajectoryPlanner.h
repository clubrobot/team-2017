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

	float getLinearVelocitySetpoint() const;
	float getAngularVelocitySetpoint() const;

	bool hasReachedItsTarget() const;

	bool addWaypoint(const Position& waypoint);
	void reset();
	
	void setCartesianPositionInput(const Position& position);

	void setLinearVelocityTunings (float Kp, float max);
	void setAngularVelocityTunings(float Kp, float max);
	void setBezierCurveParameters(float alpha, float beta);
	void setThresholdRadius(float radius);
	void setThresholdPositions(float linearPosition, float angularPosition);
	
private:

	virtual void process(float timestep);

	Position m_cartesianPositionInput;
	float    m_linearVelocitySetpoint;
	float    m_angularVelocitySetpoint;

	Position m_waypoints[TRAJECTORYPLANNER_MAX_WAYPOINTS];
	int      m_remainingWaypoints;

	// Persistant trajectory data
	bool m_turnOnTheSpot;
	bool m_targetReached;

	// Linear and angular velocities tunings
	float m_linearVelocityKp;
	float m_angularVelocityKp;
	float m_maxLinearVelocity;
	float m_maxAngularVelocity;

	// Thresholds
	float m_thresholdRadius;
	float m_thresholdLinearPosition;
	float m_thresholdAngularPosition;
};

#endif // __TRAJECTORYPLANNER_H__
