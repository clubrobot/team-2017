#include <Arduino.h>
#include <EEPROM.h>

#include "PositionController.h"
#include "SerialTalks.h"


void PositionController::setMoveStrategy(AbstractMoveStrategy& moveStrategy)
{
	m_moveStrategy = &moveStrategy;
	m_moveStrategy->m_context = this;
}

void PositionController::process(float timestep)
{
	if (m_moveStrategy != 0)
		m_moveStrategy->computeVelSetpoints(timestep);
}

bool PositionController::getPositionReached()
{
	if (m_moveStrategy != 0)
		return m_moveStrategy->getPositionReached();
	return false;
}

void PositionController::load(int address)
{
	EEPROM.get(address, m_linVelKp); address += sizeof(m_linVelKp);
	EEPROM.get(address, m_angVelKp); address += sizeof(m_angVelKp);
	EEPROM.get(address, m_linVelMax); address += sizeof(m_linVelMax);
	EEPROM.get(address, m_angVelMax); address += sizeof(m_angVelMax);
	EEPROM.get(address, m_linPosThreshold); address += sizeof(m_linPosThreshold);
	EEPROM.get(address, m_angPosThreshold); address += sizeof(m_angPosThreshold);
}

void PositionController::save(int address) const
{
	EEPROM.put(address, m_linVelKp); address += sizeof(m_linVelKp);
	EEPROM.put(address, m_angVelKp); address += sizeof(m_angVelKp);
	EEPROM.put(address, m_linVelMax); address += sizeof(m_linVelMax);
	EEPROM.put(address, m_angVelMax); address += sizeof(m_angVelMax);
	EEPROM.put(address, m_linPosThreshold); address += sizeof(m_linPosThreshold);
	EEPROM.put(address, m_angPosThreshold); address += sizeof(m_angPosThreshold);
}

