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

void TrajectoryPlanner::computeProjection(const Position& pos)
{
	m_projection = m_waypoints[m_numWaypoints - 1];
	m_edgeIndex = 0;
	m_projectionDistance = INFINITY;
	for (int i = 0; i < m_numWaypoints - 1; i++)
	{
		const float dx = pos.x - m_waypoints[i].x;
		const float dy = pos.y - m_waypoints[i].y;
		const float edgedx = m_waypoints[i+1].x - m_waypoints[i].x;
		const float edgedy = m_waypoints[i+1].y - m_waypoints[i].y;
		float h, t = (edgedx * dx + edgedy * dy) / (edgedx * edgedx + edgedy * edgedy);
		if (t < 1)
		{
			if (t < 0)
			{
				t = 0;
				h = sqrt(dx * dx + dy * dy);
			}
			else
			{
				h = abs(edgedx * dy - edgedy * dx) / sqrt(edgedx * edgedx + edgedy * edgedy);
			}
			if (h < m_projectionDistance)
			{
				m_projectionDistance = h;
				m_projection.x = m_waypoints[i].x + t * edgedx;
				m_projection.y = m_waypoints[i].y + t * edgedy;
				m_edgeIndex = i;
			}
		}
	}
}

void TrajectoryPlanner::computeGoal(const Position& pos)
{
	m_goal = m_waypoints[m_numWaypoints - 1];
	bool foundGoal = false;
	for (int i = m_edgeIndex; i < m_numWaypoints - 1; i++)
	{
		const float dx = pos.x - m_waypoints[i].x;
		const float dy = pos.y - m_waypoints[i].y;
		const float edgedx = m_waypoints[i+1].x - m_waypoints[i].x;
		const float edgedy = m_waypoints[i+1].y - m_waypoints[i].y;
		const float edgeLength = sqrt(edgedx * edgedx + edgedy * edgedy);
		float t, h = (edgedx * dy - edgedy * dx) / edgeLength;
		if (m_lookAhead * m_lookAhead > h * h)
		{
			t = (edgedx * dx + edgedy * dy) / (edgeLength * edgeLength);
			t += sqrt(m_lookAhead * m_lookAhead - h * h) / edgeLength;
			if (t >= 0 && t <= 1)
			{
				m_goal.x = m_waypoints[i].x + t * edgedx;
				m_goal.y = m_waypoints[i].y + t * edgedy;
				foundGoal = true;
			}
			else if (foundGoal)
				break;
		}
	}
}

void TrajectoryPlanner::process(float timestep)
{
	// Compute the target point
	computeProjection(m_posInput);
	if (m_projectionDistance < m_lookAhead)
		computeGoal(m_posInput);
	else
		m_goal = m_projection;

	// Remove the skipped waypoints
	for (int i = m_edgeIndex; i < m_numWaypoints; i++)
		m_waypoints[i - m_edgeIndex] = m_waypoints[i];
	m_numWaypoints -= m_edgeIndex;

	// Compute the setpoint velocitites
	float delta = atan2(m_goal.y - m_posInput.y, m_goal.x - m_posInput.x) - m_posInput.theta;
	delta = inrange(delta, -M_PI, M_PI);
	m_linVelSetpoint = m_linVelMax * saturate(cos(delta), 0, 1);
	m_angVelSetpoint = saturate(m_angVelKp * delta, -m_angVelMax, m_angVelMax);
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
