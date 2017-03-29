#include <Arduino.h>
#include <EEPROM.h>

#include "SmoothTrajectory.h"
#include "SerialTalks.h"
#include "mathutils.h"


void SmoothTrajectory::reset()
{
	m_underPosThresholds = false;
}

void SmoothTrajectory::computeVelSetpoints(float timestep)
{
	// Aliases
	const Position& current = getPosInput();
	const Position& target  = getPosSetpoint();
	const float linVelKp = getLinVelKp();
	const float angVelKp = getAngVelKp();
	const float linVelMax = getLinVelMax();
	const float angVelMax = getAngVelMax();

	// Use the robot frame of reference 
	const float dx = target.x - current.x;
	const float dy = target.y - current.y;
	const float du =  cos(current.theta) * dx + sin(current.theta) * dy;
	const float dv = -sin(current.theta) * dx + cos(current.theta) * dy;
	const float theta = target.theta - current.theta;

	// Compute the oriented distance between the robot and its target
	float chord = sqrt(du * du + dv * dv);
	float delta = atan2(dv, du);

	// Are we under the threshold radius?
	bool underThresholdRadius = chord < m_thresholdRadius * 2 * abs(sin(delta - theta));

	// Compute the circular arc distance to be traveled in order to reach the destination
	// The robot may move backward depending on which circular arc it is located
	float linPosSetpoint;
	float angPosSetpoint;
	if (underThresholdRadius || sin(delta - theta) == 0)
	{
		linPosSetpoint = du;
		angPosSetpoint = inrange(theta, -M_PI, M_PI);
	}
	else
	{
		float circArcAngle = inrange(2 * (delta - theta), -M_PI, M_PI);
		linPosSetpoint = chord * circArcAngle / (2 * sin(delta - theta));
		angPosSetpoint = inrange(theta + 2 * (delta - theta), -M_PI, M_PI);
	}

	// Let the robot turn on the spot if it is not well oriented
	if (!underThresholdRadius && cos(angPosSetpoint) < 0)
		linPosSetpoint = 0;

	// It is more convenient to compute this here than in the `getPositionReached` method.
	m_underPosThresholds = 
		(abs(linPosSetpoint) < getLinPosThreshold()) &&
		(abs(angPosSetpoint) < getAngPosThreshold());

	// Control velocities separately
	float linVelSetpoint = saturate(linVelKp * linPosSetpoint, -linVelMax, linVelMax);
	float angVelSetpoint = saturate(angVelKp * angPosSetpoint, -angVelMax, angVelMax);
	setVelSetpoints(linVelSetpoint, angVelSetpoint);
}

bool SmoothTrajectory::getPositionReached()
{
	return m_underPosThresholds;
}

void SmoothTrajectory::load(int address)
{
	EEPROM.get(address, m_thresholdRadius); address += sizeof(m_thresholdRadius);
}

void SmoothTrajectory::save(int address) const
{
	EEPROM.put(address, m_thresholdRadius); address += sizeof(m_thresholdRadius);
}
