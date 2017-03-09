#include <Arduino.h>
#include <EEPROM.h>

#include "DifferentialController.h"
#include "SerialTalks.h"


void DifferentialController::process(float timestep)
{
	// Compute linear and angular velocities outputs
	float linVelOutput = m_linPID->compute(m_linSetpoint, m_linInput, timestep);
	float angVelOutput = m_angPID->compute(m_angSetpoint, m_angInput, timestep);

#if OUTPUT_CONTROL_VARIABLES
	talks.out << millis() << "\t";
	talks.out << m_linSetpoint << "\t" << m_linInput << "\t" << linVelOutput << "\t";
	talks.out << m_angSetpoint << "\t" << m_angInput << "\t" << angVelOutput << "\n";
#endif

	// Convert linear and angular velocities into wheels' velocities
	m_leftWheel ->setVelocity(linVelOutput - angVelOutput * m_axleTrack / 2);
	m_rightWheel->setVelocity(linVelOutput + angVelOutput * m_axleTrack / 2);
}

void DifferentialController::onProcessEnabling()
{
	m_linPID->reset();
	m_angPID->reset();
}

void DifferentialController::load(int address)
{
	EEPROM.get(address, m_axleTrack); address += sizeof(m_axleTrack);
}

void DifferentialController::save(int address) const
{
	EEPROM.put(address, m_axleTrack); address += sizeof(m_axleTrack);
}
