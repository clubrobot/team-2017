#include "DCMotor.h"

DCMotor::DCMotor(int enablePin, int input1Pin, int input2Pin)
:	m_enable(false)
,	m_ratio(0)
,	m_driverMode(FAST_DECAY)

,	m_enablePin(enablePin)
,	m_input1Pin(input1Pin)
,	m_input2Pin(input2Pin)
{
	pinMode(m_enablePin, OUTPUT);
	pinMode(m_input1Pin, OUTPUT);
	pinMode(m_input2Pin, OUTPUT);
	updatePins();
}

void DCMotor::enable(bool enable)
{
	m_enable = enable;
	updatePins();
}

void DCMotor::setSpeed(float ratio)
{
	if (ratio < -1.0)
		m_ratio = -1.0;
	else if (ratio > 1.0)
		m_ratio = 1.0;
	else
		m_ratio = ratio;
	updatePins();
}

void DCMotor::setDriverMode(DriverMode mode)
{
	m_driverMode = mode;
	updatePins();
}

void DCMotor::updatePins()
{
	if (m_enable && m_ratio != 0)
	{
		if (m_driverMode == FAST_DECAY && m_ratio > 0)
		{
			analogWrite(m_enablePin, int(255 * m_ratio));
			digitalWrite(m_input1Pin, HIGH);
			digitalWrite(m_input2Pin, LOW);
		}
		else if (m_driverMode == FAST_DECAY && m_ratio < 0)
		{
			analogWrite(m_enablePin, -int(255 * m_ratio));
			digitalWrite(m_input1Pin, LOW);
			digitalWrite(m_input2Pin, HIGH);
		}
		else if (m_driverMode == SLOW_DECAY && m_ratio > 0)
		{
			digitalWrite(m_enablePin, HIGH);
			analogWrite(m_input1Pin, int(255 * m_ratio));
			analogWrite(m_input2Pin, 0);
		}
		else if (m_driverMode == SLOW_DECAY && m_ratio < 0)
		{
			digitalWrite(m_enablePin, HIGH);
			analogWrite(m_input1Pin, 0);
			analogWrite(m_input2Pin, -int(255 * m_ratio));
		}
	}
	else
	{
		digitalWrite(m_enablePin, LOW);
	}
}

DCDriver::DCDriver(int resetPin, int faultPin)
:	m_resetPin(resetPin)
,	m_faultPin(faultPin)
{
	pinMode(m_resetPin, OUTPUT);
	pinMode(m_faultPin, INPUT);
	reset();
}

void DCDriver::reset()
{
	digitalWrite(m_resetPin, LOW);
	delayMicroseconds(10); // One may adjust this value.
	digitalWrite(m_resetPin, HIGH);
}

bool DCDriver::isFaulty()
{
	return (digitalRead(m_faultPin) == LOW);
}
