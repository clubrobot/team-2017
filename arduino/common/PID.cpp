#include "PID.h"
#include <Arduino.h>
#include <EEPROM.h>


PID::PID(int address)
:	m_address(address)
{
	loadTunings();
	reset();
}

PID::PID(int address, float Kp, float Ki, float Kd)
:	m_Kp(Kp)
,	m_Ki(Ki)
,	m_Kd(Kd)

,	m_address(address)
{
	saveTunings();
	reset();
}

bool PID::compute(float setpoint, float input, float& output)
{
	if (m_clock.getElapsedTime() > m_timestep)
	{
		// Compute the elapsed time since the last call
		float timestep = m_clock.restart();
		if (timestep > 2 * m_timestep)
			timestep = m_timestep;

		// Compute the error between the current state and the setpoint
		float currentError = setpoint - input;

		// Compute the error integral
		m_errorIntegral += currentError * timestep;

		// Compute the error derivative
		float errorDerivative = (currentError - m_previousError) / timestep;
		m_previousError = currentError;

		// Compute the PID controller's output
		output = m_Kp * currentError + m_Ki * m_errorIntegral - m_Kd * errorDerivative;
		return true;
	}
	return false;
}

void PID::setTunings(float Kp, float Ki, float Kd)
{
	m_Kp = Kp;
	m_Ki = Ki;
	m_Kd = Kd;
	saveTunings();
}

void PID::setTimestep(float timestep)
{
	m_timestep = timestep;
}

void PID::reset()
{
	m_errorIntegral = 0;
	m_previousError = 0;
	m_clock.restart();
}

void PID::loadTunings()
{
	int address = m_address;
	EEPROM.get(address, m_Kp); address += sizeof(m_Kp);
	EEPROM.get(address, m_Ki); address += sizeof(m_Ki);
	EEPROM.get(address, m_Kd); address += sizeof(m_Kd);
}

void PID::saveTunings() const
{
	int address = m_address;
	EEPROM.put(address, m_Kp); address += sizeof(m_Kp);
	EEPROM.put(address, m_Ki); address += sizeof(m_Ki);
	EEPROM.put(address, m_Kd); address += sizeof(m_Kd);
}
