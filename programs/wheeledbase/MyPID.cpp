#include <math.h>
#include <EEPROM.h>
#include "MyPID.h"


MyPID::MyPID(const int address)
:	PID(&m_input, &m_output, &m_setpoint, 0, 0, 0, DIRECT)
,	m_input(0)
,	m_output(0)
,	m_setpoint(0)

,	m_address(address)
{
	PID::SetOutputLimits(-INFINITY, INFINITY);
	loadTunings();
	enable();
}

void MyPID::setTunings(float Kp, float Ki, float Kd)
{
	PID::SetTunings(Kp, Ki, Kd);
	saveTunings();
}

void MyPID::loadTunings()
{
	float Kp = 0, Ki = 0, Kd = 0;
	int address = m_address;
	EEPROM.get(address, Kp); address += sizeof(Kp);
	EEPROM.get(address, Ki); address += sizeof(Ki);
	EEPROM.get(address, Kd); address += sizeof(Kd);
	PID::SetTunings(Kp, Ki, Kd);
}

void MyPID::saveTunings() const
{
	float Kp = getKp(), Ki = getKi(), Kd = getKd();
	int address = m_address;
	EEPROM.put(address, Kp); address += sizeof(Kp);
	EEPROM.put(address, Ki); address += sizeof(Ki);
	EEPROM.put(address, Kd); address += sizeof(Kd);
}
