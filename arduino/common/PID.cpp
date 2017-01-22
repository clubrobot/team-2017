#include "PID.h"
#include "SerialTalks.h"
#include <Arduino.h>
#include <EEPROM.h>
#include <math.h>


bool PID::compute(float setpoint, float input, float& output)
{
	bool updated = m_clock.getElapsedTime() > m_timestep;
	if (updated)
	{
		// Compute the elapsed time since the last computation
		float timestep = m_clock.restart();

		// If this time is more than 2 times the timestep, we consider that the user manually
		// disabled the controller. So we reset it in order to avoid aberrations.
		if (timestep > 2 * m_timestep)
		{
			m_output = 0;
			m_errorIntegral = 0;
			m_previousError = setpoint - input;
		}

		// Else we compute the output depending on the current error and the previously computed
		// variables.
		else
		{
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
			if m_output > m_maxOutput
				m_output = m_maxOutput;
			else if m_output < m_minOutput
				m_output = m_minOutput;
		}
	}

	// Update output variable
	output = m_output;
	return updated;
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
	m_output = 0;
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
