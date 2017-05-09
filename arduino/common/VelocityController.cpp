#include <Arduino.h>
#include <EEPROM.h>

#include "VelocityController.h"
#include "SerialTalks.h"
#include "mathutils.h"


float VelocityController::genRampSetpoint(float stepSetpoint, float input, float rampSetpoint, float maxAcc, float maxDec, float timestep)
{
	// If we are above the desired setpoint (i.e. the ramp), we no longer try to follow it.
	// Instead we generate a new ramp starting from our current position.
	if ((input - rampSetpoint) * (stepSetpoint - rampSetpoint) > 0)
		rampSetpoint = input;

	// Do we have to accelerate or deccelerate to reach the desired setpoint?
	if (input * (stepSetpoint - input) >= 0)
		rampSetpoint += sign(stepSetpoint - input) * maxAcc * timestep;
	else
		rampSetpoint += sign(stepSetpoint - input) * maxDec * timestep;

	// We clamp the ramp so that it never exceeds the real setpoint
	if ((stepSetpoint - input) * (stepSetpoint - rampSetpoint) < 0) // TODO: test if this condition runs well when rampSetpoint is set to INFINITY
		rampSetpoint = stepSetpoint;

	return rampSetpoint;
}

void VelocityController::process(float timestep)
{
	// Save setpoints
	const float stepLinVelSetpoint = m_linSetpoint;
	const float stepAngVelSetpoint = m_angSetpoint;

	// Compute new setpoints
	m_rampLinVelSetpoint = genRampSetpoint(m_linSetpoint, m_linInput, m_rampLinVelSetpoint, m_maxLinAcc, m_maxLinDec, timestep);
	m_rampAngVelSetpoint = genRampSetpoint(m_angSetpoint, m_angInput, m_rampAngVelSetpoint, m_maxAngAcc, m_maxAngDec, timestep);

	// Do the engineering control
	m_linSetpoint = m_rampLinVelSetpoint;
	m_angSetpoint = m_rampAngVelSetpoint;
	DifferentialController::process(timestep);

	// Check for wheels abnormal spin and stop the controller accordingly
	const bool saturatedLinVelOutput = (m_linVelOutput == m_linPID->getMinOutput() || m_linVelOutput == m_linPID->getMaxOutput());
	const bool saturatedAngVelOutput = (m_angVelOutput == m_angPID->getMinOutput() || m_angVelOutput == m_angPID->getMaxOutput());
	const bool linSpinUrgency = saturatedLinVelOutput && abs(m_linInput) <    1; // linear velocity < 1 cm/s
	const bool angSpinUrgency = saturatedAngVelOutput && abs(m_angInput) < 0.05; // angular velocity < 0.05 rad/s
	if (linSpinUrgency || angSpinUrgency)
	{
		talks.out << linSpinUrgency << ", " << angSpinUrgency << "\n";
		talks.out << m_linVelOutput << ", " << m_linInput << m_linPID->getMaxOutput() << "\n";
		talks.out << m_angVelOutput << ", " << m_angInput << m_angPID->getMaxOutput() << "\n";
		m_leftWheel ->setVelocity(0);
		m_rightWheel->setVelocity(0);
		disable();
	}
		
	// Restore setpoints
	m_linSetpoint = stepLinVelSetpoint;
	m_angSetpoint = stepAngVelSetpoint;
}

void VelocityController::onProcessEnabling()
{
	DifferentialController::onProcessEnabling();
	m_rampLinVelSetpoint = 0;
	m_rampAngVelSetpoint = 0;
}

void VelocityController::load(int address)
{
	EEPROM.get(address, m_axleTrack); address += sizeof(m_axleTrack);
	EEPROM.get(address, m_maxLinAcc); address += sizeof(m_maxLinAcc);
	EEPROM.get(address, m_maxLinDec); address += sizeof(m_maxLinDec);
	EEPROM.get(address, m_maxAngAcc); address += sizeof(m_maxAngAcc);
	EEPROM.get(address, m_maxAngDec); address += sizeof(m_maxAngDec);
}

void VelocityController::save(int address) const
{
	EEPROM.put(address, m_axleTrack); address += sizeof(m_axleTrack);
	EEPROM.put(address, m_maxLinAcc); address += sizeof(m_maxLinAcc);
	EEPROM.put(address, m_maxLinDec); address += sizeof(m_maxLinDec);
	EEPROM.put(address, m_maxAngAcc); address += sizeof(m_maxAngAcc);
	EEPROM.put(address, m_maxAngDec); address += sizeof(m_maxAngDec);
}
