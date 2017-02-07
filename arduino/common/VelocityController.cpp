#include "VelocityController.h"
#include "mathutils.h"
#include "SerialTalks.h"


void VelocityController::setMaximumAccelerations(float linearAcceleration, float angularAcceleration)
{
	m_maxLinearAcceleration  = linearAcceleration;
	m_maxAngularAcceleration = angularAcceleration;
}

void VelocityController::setMaximumDeccelerations(float linearDecceleration, float angularDecceleration)
{
	m_maxLinearDecceleration  = linearDecceleration;
	m_maxAngularDecceleration = angularDecceleration;
}

void VelocityController::process(float timestep)
{
	// Save setpoints
	const float linearVelocitySetpoint  = m_linearSetpoint;
	const float angularVelocitySetpoint = m_angularSetpoint;

	// Compute new setpoints
	if (m_linearInput >= 0)
	{
		const float minLinearSetpoint = m_linearInput - m_maxLinearDecceleration * timestep;
		const float maxLinearSetpoint = m_linearInput + m_maxLinearAcceleration  * timestep;
		m_linearSetpoint = saturate(m_linearSetpoint, minLinearSetpoint, maxLinearSetpoint);
	}
	else
	{
		const float minLinearSetpoint = m_linearInput - m_maxLinearAcceleration  * timestep;
		const float maxLinearSetpoint = m_linearInput + m_maxLinearDecceleration * timestep;
		m_linearSetpoint = saturate(m_linearSetpoint, minLinearSetpoint, maxLinearSetpoint);
	}
	if (m_angularInput >= 0)
	{
		const float minAngularSetpoint = m_angularInput - m_maxAngularDecceleration * timestep;
		const float maxAngularSetpoint = m_angularInput + m_maxAngularAcceleration  * timestep;
		m_angularSetpoint = saturate(m_angularSetpoint, minAngularSetpoint, maxAngularSetpoint);
	}
	else
	{
		const float minAngularSetpoint = m_angularInput - m_maxAngularAcceleration  * timestep;
		const float maxAngularSetpoint = m_angularInput + m_maxAngularDecceleration * timestep;
		m_angularSetpoint = saturate(m_angularSetpoint, minAngularSetpoint, maxAngularSetpoint);
	}

	// Do the engineering control
	DifferentialController::process(timestep);

	// Restore setpoints
	m_linearSetpoint  = linearVelocitySetpoint;
	m_angularSetpoint = angularVelocitySetpoint;
}
