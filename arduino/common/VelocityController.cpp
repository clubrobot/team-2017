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

float VelocityController::generateRampSetpoint(float stepSetpoint, float input, float rampSetpoint, float acceleration, float decceleration, float timestep)
{
	// Do we have to accelerate or deccelerate to reach the desired setpoint?
	if (input * (stepSetpoint - input) >= 0)
		rampSetpoint += sign(stepSetpoint - input) * acceleration * timestep;
	else
		rampSetpoint += sign(stepSetpoint - input) * decceleration * timestep;

	// If we are above the desired setpoint (i.e. the ramp), we no longer try to follow it.
	// Instead we generate a new ramp starting from our current position.
	if ((input - rampSetpoint) * (stepSetpoint - rampSetpoint) > 0)
		rampSetpoint = input;

	// We clamp the ramp so that it never exceeds the real setpoint
	if ((stepSetpoint - input) * (stepSetpoint - rampSetpoint) < 0)
		rampSetpoint = stepSetpoint;
	
	return rampSetpoint;
}

void VelocityController::process(float timestep)
{
	// Save setpoints
	const float savedLinearSetpoint  = m_linearSetpoint;
	const float savedAngularSetpoint = m_angularSetpoint;

	// Compute new setpoints
	m_transitionalLinearVelocitySetpoint  = generateRampSetpoint(m_linearSetpoint,  m_linearInput,  m_transitionalLinearVelocitySetpoint,  m_maxLinearAcceleration,  m_maxLinearDecceleration,  timestep);
	m_transitionalAngularVelocitySetpoint = generateRampSetpoint(m_angularSetpoint, m_angularInput, m_transitionalAngularVelocitySetpoint, m_maxAngularAcceleration, m_maxAngularDecceleration, timestep);
//	m_transitionalAngularVelocitySetpoint = m_angularSetpoint;

	// Do the engineering control
	m_linearSetpoint  = m_transitionalLinearVelocitySetpoint;
	m_angularSetpoint = m_transitionalAngularVelocitySetpoint;
	DifferentialController::process(timestep);

	// Restore setpoints
	m_linearSetpoint  = savedLinearSetpoint;
	m_angularSetpoint = savedAngularSetpoint;
}
