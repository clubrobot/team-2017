#include "PID.h"
#include "SerialTalks.h"
#include <Arduino.h>
#include <EEPROM.h>
#include <math.h>


bool PID::compute(float setpoint, float input, float& output)
{
	if (m_clock.getElapsedTime() > m_timestep)
	{
		// Compute the elapsed time since the last call
		float timestep = m_clock.restart();
		if (timestep > 2 * m_timestep)
		{
			reset();
			m_output = 0;
			output = m_output;
			return false;
		}
		
		// Compute the error between the current state and the setpoint
		float currentError = setpoint - input;

		// Compute the error integral
		m_errorIntegral += currentError * timestep;
		if (m_errorIntegral > m_maxOutput / m_Ki)
			m_errorIntegral = m_maxOutput / m_Ki;
		else if (m_errorIntegral < m_minOutput / m_Ki)
			m_errorIntegral = m_minOutput / m_Ki;

		// Compute the error derivative
		float errorDerivative = (currentError - m_previousError) / timestep;
		m_previousError = currentError;

		// Compute the PID controller's output
		m_output = m_Kp * currentError + m_Ki * m_errorIntegral - m_Kd * errorDerivative;
		output = m_output;
		return true;
	}
	output = m_output;
	return false;
}

void PID::setTunings(float Kp, float Ki, float Kd)
{
	m_Kp = Kp;
	m_Ki = Ki;
	m_Kd = Kd;
}

void PID::setOutputLimits(float minOutput, float maxOutput)
{
	m_minOutput = minOutput;
	m_maxOutput = maxOutput;
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

void PID::loadTunings(int address)
{
	EEPROM.get(address, m_Kp); address += sizeof(m_Kp);
	EEPROM.get(address, m_Ki); address += sizeof(m_Ki);
	EEPROM.get(address, m_Kd); address += sizeof(m_Kd);
}

void PID::saveTunings(int address) const
{
	EEPROM.put(address, m_Kp); address += sizeof(m_Kp);
	EEPROM.put(address, m_Ki); address += sizeof(m_Ki);
	EEPROM.put(address, m_Kd); address += sizeof(m_Kd);
}
