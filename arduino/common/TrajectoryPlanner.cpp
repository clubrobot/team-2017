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
	m_goalIndex = 0;
	m_goalParam = 0;
}

bool TrajectoryPlanner::checkLookAheadGoal()
{
	// The purpose of this function is to find the intersection point between a circle of radius
	// `m_lookAhead` centered at the robot position and the path. As there may be several of them,
	// we iterate through the path segments in the order of passing and stop as soon as we find
	// one.
	for (int i = m_goalIndex; i < m_numWaypoints-1; i++)
	{
		float dx = m_posInput.x - m_waypoints[i].x;
		float dy = m_posInput.y - m_waypoints[i].y;
		float edgedx = m_waypoints[i+1].x - m_waypoints[i].x;
		float edgedy = m_waypoints[i+1].y - m_waypoints[i].y;
		float edgeLength = sqrt(edgedx * edgedx + edgedy * edgedy);

		// `h` is the distance between the robot and the current line (i.e. the current segment but
		// without regard to its endpoints).
		float h = abs(edgedx * dy - edgedy * dx) / edgeLength;
		if (m_lookAhead < h) // There is no intersection between the current line and the circle.
			continue;
		
		// `t` is the relative position of the robot projection in the current segment.
		// `t = 0` means that the robot projection is on its first endpoint and `t = 1` means
		// that the robot projection is on its second endpoint.
		float t = (edgedx * dx + edgedy * dy) / (edgeLength * edgeLength);

		// There is two intersection points between the circle and the line.
		// For all but the last segment, we only consider the one ahead of the robot: if it is
		// beyond the second endpoint (so it's not on the segment), then we go to the next.
		if (t < 1 || i+1 < m_numWaypoints-1)
			t += sqrt(m_lookAhead * m_lookAhead - h * h) / edgeLength;
		
		// However for the last segment, we want to clamp it if it is beyond the second endpoint.
		// But we want to do so only if the disk still intersects with the segment. So we must
		// consider the backward intersection point and make sure it is not beyond the second
		// endpoint too (otherwise it means that the circle does not intersect with the segment).
		else if (t - sqrt(m_lookAhead * m_lookAhead - h * h) / edgeLength > 1)
			continue;
		
		// Skip if the intersection point is beyond the second endpoint (see above).
		if (t < 0 || (t > 1 && i+1 < m_numWaypoints-1))
			continue;
		
		if (t > 1) // The goal is beyond the last waypoint so we clamp it.
		{
			t = 0;
			i = i+1;
		}

		// Save the new goal.
		if (i > m_goalIndex || t > m_goalParam)
		{
			m_goalIndex = i;
			m_goalParam = t;
		}
		return true;
	}
	return false;
}

void TrajectoryPlanner::checkProjectionGoal()
{
	// The purpose of this function is to find the closest point between the path and the robot. To
	// do so we iterate through all the segments, cmpute their distance to the robot and sort them.
	float hmin = INFINITY;
	for (int i = m_goalIndex; i < m_numWaypoints-1; i++)
	{
		float dx = m_posInput.x - m_waypoints[i].x;
		float dy = m_posInput.y - m_waypoints[i].y;
		float edgedx = m_waypoints[i+1].x - m_waypoints[i].x;
		float edgedy = m_waypoints[i+1].y - m_waypoints[i].y;
		float edgeLength = sqrt(edgedx * edgedx + edgedy * edgedy);

		// `t` and `h` have the same meaning than in the `checkLookAheadGoal` method.
		float h, t = (edgedx * dx + edgedy * dy) / (edgeLength * edgeLength);
		if (t > 1 && i+1 < m_numWaypoints-1)
			continue;
		
		if (t > 1) // The closest point of the segment is its second endpoint.
		{
			dx = m_posInput.x - m_waypoints[i+1].x;
			dy = m_posInput.y - m_waypoints[i+1].y;
			t = 0;
			i = i+1;
		}

		if (t <= 0) // The closest point of the segment is its first endpoint.
		{
			h = sqrt(dx * dx + dy * dy);
			t = 0;
		}
		else
		{
			h = abs(edgedx * dy - edgedy * dx) / edgeLength;
		}

		// Save the new projection point if it is closer than the current one.
		if (h < hmin)
		{
			hmin = h;
			if (i > m_goalIndex || t > m_goalParam)
			{
				m_goalIndex = i;
				m_goalParam = t;
			}
		}
	}
}

float TrajectoryPlanner::getDistAfterGoal()
{
	// This function computes the remaining distance between the current goal and the last
	// waypoint. It basically does the sum of all the last segments lengths.
	float dist = 0;
	for (int i = m_goalIndex; i < m_numWaypoints-1; i++)
	{
		float edgedx = m_waypoints[i+1].x - m_waypoints[i].x;
		float edgedy = m_waypoints[i+1].y - m_waypoints[i].y;
		float edgeLength = sqrt(edgedx * edgedx + edgedy * edgedy);
		if (i == m_goalIndex)
			dist += (1 - m_goalParam) * edgeLength;
		else
			dist += edgeLength;
	}
	return dist;
}

void TrajectoryPlanner::process(float timestep)
{
	// Compute the goal position on the path: `m_goalIndex` is the index of the segment it is
	// located on and `m_goalParam` is its relative position on that segment.
	if (!checkLookAheadGoal())
		checkProjectionGoal();

	// Compute the goal cartesian position.
	Position goal = m_waypoints[m_numWaypoints-1];
	if (m_goalIndex < m_numWaypoints - 1)
	{
		int i = m_goalIndex;
		float t = m_goalParam;
		goal.x = (1-t) * m_waypoints[i].x + t * m_waypoints[i+1].x;
		goal.y = (1-t) * m_waypoints[i].y + t * m_waypoints[i+1].y;
	}

	// Compute the norm and the argument of the vector going from the robot to its goal.
	float chord = sqrt((goal.x - m_posInput.x) * (goal.x - m_posInput.x) + (goal.y - m_posInput.y) * (goal.y - m_posInput.y));
	float delta = atan2(goal.y - m_posInput.y, goal.x - m_posInput.x) - m_posInput.theta;

	// Compute the remaining distance and an appropriate velocity setpoint.
	float linPosSetpoint = (chord + getDistAfterGoal()) * sign(cos(delta));
	m_linVelSetpoint = saturate(m_linVelKp * linPosSetpoint, -m_linVelMax, m_linVelMax);

	// The curvature that the robot must follow is `2 * sin(delta) / chord`. So we deduce from this
	// and the linear velocity setpoint the angular velocity setpoint. If it exceeds the maximum
	// angular velocity, then we do the opposite: we set the angular velocity setpoint and deduce
	// from it and the previous formula the linear velocity setpoint.
	if (m_angVelMax * chord >= 2 * abs(m_linVelSetpoint * sin(delta)))
	{
		m_angVelSetpoint = m_linVelSetpoint * (2 * sin(delta)) / chord;
	}
	else
	{
		m_angVelSetpoint = m_angVelMax * sign(m_linVelSetpoint * sin(delta));
		m_linVelSetpoint = m_angVelSetpoint * chord / (2 * sin(delta));
	}
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
