#include "DCMotor.h"

DCMotor::DCMotor(int enablePin, int input1Pin, int input2Pin)
:	m_enable(false)
,	m_power(0)
,	m_driverMode(FAST_DECAY)

,	m_enablePin(enablePin)
,	m_input1Pin(input1Pin)
,	m_input2Pin(input2Pin)
{
	pinMode(m_enablePin, OUTPUT);
	pinMode(m_input1Pin, OUTPUT);
	pinMode(m_input2Pin, OUTPUT);
	digitalWrite(m_enablePin, LOW);
}

void DCMotor::enable(bool enable)
{
	m_enable = enable;
	updatePins();
}

void DCMotor::setSpeed(float ratio)
{
	if (power < -1.0)
		m_power = -1.0;
	else if (power > 1.0)
		m_power = 1.0;
	else
		m_power = power;
	updatePins();
}

void DCMotor::setDriverMode(DriverMode mode)
{
	m_driverMode = mode;
	updatePins();
}

void DCMotor::updatePins()
{
	if (m_enable && m_power != 0)
	{
		if (m_driverMode == FAST_DECAY && m_power > 0)
		{
			analogWrite(m_enablePin, int(255 * power));
			digitalWrite(m_input1Pin, HIGH);
			digitalWrite(m_input2Pin, LOW);
		}
		else if (m_driverMode == FAST_DECAY && m_power < 0)
		{
			analogWrite(m_enablePin, -int(255 * power);
			digitalWrite(m_input1Pin, LOW);
			digitalWrite(m_input2Pin, HIGH);
		}
		else if (m_driverMode == SLOW_DECAY && m_power > 0)
		{
			digitalWrite(m_enablePin, HIGH);
			analogWrite(m_input1Pin, int(255 * power));
			analogWrite(m_input2Pin, 0);
		}
		else if (m_driverMode == SLOW_DECAY && m_power < 0)
		{
			digitalWrite(m_enablePin, HIGH);
			analogWrite(m_input1Pin, 0);
			analogWrite(m_input2Pin, -int(255 * power));
		}
	}
	else
	{
		digitalWrite(m_enablePin, LOW);
	}
}
