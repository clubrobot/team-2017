#ifndef __PUREPURSUIT_H__
#define __PUREPURSUIT_H__

#include "PositionController.h"
#include "Odometry.h"

#include <math.h>

#ifndef PUREPURSUIT_MAX_WAYPOINTS
#define PUREPURSUIT_MAX_WAYPOINTS 16
#endif


class PurePursuit : public AbstractMoveStrategy
{
public:

	struct Waypoint
	{
		Waypoint() : x(0), y(0){}
		Waypoint(float x, float y) : x(x), y(y){}
		Waypoint(const Position& pos) : x(pos.x), y(pos.y){}

		float x, y;
	};

	PurePursuit() : m_numWaypoints(0){}

	bool addWaypoint(const Waypoint& waypoint);

	void reset();

	void setLookAhead(float lookAhead){m_lookAhead = lookAhead;}

	float getLookAhead() const {return m_lookAhead;}

	void load(int address);
	void save(int address) const;

protected:

	virtual void computeVelSetpoints(float timestep);
	virtual bool getPositionReached();

	bool checkLookAheadGoal(const float x, const float y);
	void checkProjectionGoal(const float x, const float y);
	float getDistAfterGoal();

	// Trajectory specifications
	Waypoint m_waypoints[PUREPURSUIT_MAX_WAYPOINTS];
	int m_numWaypoints;

	// Computation variables
	int m_goalIndex;
	float m_goalParam;
	bool m_goalReached;

	// Path following tunings
	float m_lookAhead;
};

#endif // __PUREPURSUIT_H__
