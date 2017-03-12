#include <Arduino.h>
#include <EEPROM.h>

#include "TrajectoryPlanner.h"
#include "SerialTalks.h"
#include "mathutils.h"


bool TrajectoryPlanner::addWaypoint(const Position& waypoint)
{
	if (m_numWaypoints < TRAJECTORYPLANNER_MAX_WAYPOINTS)
	{
		m_waypoints[m_numWaypoints++] = waypoint;
		return true;
	}
	return false;
}

void TrajectoryPlanner::reset()
{
	m_numWaypoints = 0;
}

void TrajectoryPlanner::process(float timestep)
{
	//TODO
}

void TrajectoryPlanner::load(int address)
{
	EEPROM.get(address, m_linVelKp); address += sizeof(m_linVelKp);
	EEPROM.get(address, m_angVelKp); address += sizeof(m_angVelKp);
	EEPROM.get(address, m_linVelMax); address += sizeof(m_linVelMax);
	EEPROM.get(address, m_angVelMax); address += sizeof(m_angVelMax);
	EEPROM.get(address, m_lookAhead); address += sizeof(m_lookAhead);
}

void TrajectoryPlanner::save(int address) const
{
	EEPROM.put(address, m_linVelKp); address += sizeof(m_linVelKp);
	EEPROM.put(address, m_angVelKp); address += sizeof(m_angVelKp);
	EEPROM.put(address, m_linVelMax); address += sizeof(m_linVelMax);
	EEPROM.put(address, m_angVelMax); address += sizeof(m_angVelMax);
	EEPROM.put(address, m_lookAhead); address += sizeof(m_lookAhead);
}
