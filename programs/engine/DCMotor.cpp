#include "DCMotor.h"

DCMotor::DCMotor(int EN, int IN1, int IN2, float wheelRadius, float speedConstant, int speedReductionRatio, int suppliedVoltage)
:	m_enable(true)
,	m_speed(0)
,	m_driverMode(FAST_DECAY)

,	m_enablePin(EN)
,	m_input1Pin(IN1)
,	m_input2Pin(IN2)

,	m_wheelRadius(wheelRadius)
,	m_speedConstant(speedConstant)
,	m_speedReductionRatio(speedReductionRatio)
,	m_suppliedVoltage(suppliedVoltage)
{
	pinMode(m_enablePin, OUTPUT);
	pinMode(m_input1Pin, OUTPUT);
	pinMode(m_input2Pin, OUTPUT);
	updatePins();
}

int DCMotor::getPWM() const
{
	int PWM = (60 * m_speedReductionRatio / m_speedConstant) / (2 * M_PI * m_wheelRadius) * (255 / m_suppliedVoltage);
	return (PWM >= 0) ? PWM : -PWM;
}

void DCMotor::enable(bool enable)
{
	m_enable = enable;
	updatePins();
}

void DCMotor::setSpeed(float speed)
{
	m_speed = speed;
	updatePins();
}

void DCMotor::setDriverMode(DriverMode mode)
{
	m_driverMode = mode;
	updatePins();
}

void DCMotor::updatePins()
{
	if (m_enable && m_speed != 0)
	{
		int PWM = getPWM();
		if (m_driverMode == FAST_DECAY && m_speed > 0)
		{
			analogWrite(m_enablePin, PWM);
			digitalWrite(m_input1Pin, HIGH);
			digitalWrite(m_input2Pin, LOW);
		}
		else if (m_driverMode == FAST_DECAY && m_speed < 0)
		{
			analogWrite(m_enablePin, -PWM);
			digitalWrite(m_input1Pin, LOW);
			digitalWrite(m_input2Pin, HIGH);
		}
		else if (m_driverMode == SLOW_DECAY && m_speed > 0)
		{
			digitalWrite(m_enablePin, HIGH);
			analogWrite(m_input1Pin, PWM);
			analogWrite(m_input2Pin, 0);
		}
		else if (m_driverMode == SLOW_DECAY && m_speed < 0)
		{
			digitalWrite(m_enablePin, HIGH);
			analogWrite(m_input1Pin, 0);
			analogWrite(m_input2Pin, -PWM);
		}
	}
	else
	{
		digitalWrite(m_enablePin, LOW);
	}
}

DCDriver::DCDriver(int RESET, int FAULT)
:	m_resetPin(RESET)
,	m_faultPin(FAULT)
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
