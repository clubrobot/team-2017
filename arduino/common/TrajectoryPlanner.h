#ifndef __TRAJECTORYPLANNER_H__
#define __TRAJECTORYPLANNER_H__

#include "PeriodicProcess.h"
#include "Odometry.h"

#include <math.h>

#ifndef TRAJECTORYPLANNER_MAX_WAYPOINTS
#define TRAJECTORYPLANNER_MAX_WAYPOINTS 10
#endif


class TrajectoryPlanner : public PeriodicProcess
{
public:

	TrajectoryPlanner() : m_numWaypoints(0), m_linVelKp(1), m_angVelKp(1), m_linVelMax(1000), m_angVelMax(2 * M_PI){}

	bool addWaypoint(const Position& waypoint);

	void reset();
	
	void setPosInput(const Position& posInput){m_posInput = posInput;}

	float getLinVelSetpoint() const {return m_linVelSetpoint;}
	float getAngVelSetpoint() const {return m_angVelSetpoint;}

	void setLookAhead(float lookAhead){m_lookAhead = lookAhead;}

	void setVelTunings(float linVelKp, float angVelKp) {m_linVelKp  = linVelKp;  m_angVelKp  = angVelKp;}
	void setVelLimits(float linVelMax, float angVelMax){m_linVelMax = linVelMax; m_angVelMax = angVelMax;}
	
	float getLookAhead() const {return m_lookAhead;}

	float getLinVelKp() const {return m_linVelKp;}
	float getAngVelKp() const {return m_angVelKp;}
	float getLinVelMax() const {return m_linVelMax;}
	float getAngVelMax() const {return m_angVelMax;}

	void load(int address);
	void save(int address) const;

private:

	virtual void process(float timestep);

	// I/O
	Position m_posInput;
	float m_linVelSetpoint;
	float m_angVelSetpoint;

	// Trajectory specifications
	Position m_waypoints[TRAJECTORYPLANNER_MAX_WAYPOINTS];
	int  m_numWaypoints;

	// Path following tunings
	float m_lookAhead;

	// Engineering control tunings
	float m_linVelKp;
	float m_angVelKp;
	float m_linVelMax;
	float m_angVelMax;
};

#endif // __TRAJECTORYPLANNER_H__
