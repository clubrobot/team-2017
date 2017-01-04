#include "DCMotor.h"


DCMotor::DCMotor(int EN, int PWM, int DIR, float wheelRadius, float speedConstant, int speedReductionRatio, int suppliedVoltage)
:	m_enable(true)
,	m_speed(0)

,	m_enablePin(EN)
,	m_velocityPin(PWM)
,	m_directionPin(DIR)

,	m_wheelRadius(wheelRadius)
,	m_speedConstant(speedConstant)
,	m_speedReductionRatio(speedReductionRatio)
,	m_suppliedVoltage(suppliedVoltage)
{
	pinMode(m_enablePin, OUTPUT);
	pinMode(m_velocityPin, OUTPUT);
	pinMode(m_directionPin, OUTPUT);
	updatePins();
}

int DCMotor::getPWM() const
{
	int PWM = m_speed * (60 * m_speedReductionRatio / m_speedConstant) / (2 * M_PI * m_wheelRadius) * (255 / m_suppliedVoltage);
	if (PWM > 255 || PWM < -255)
		return 255;
	else if (PWM < 0)
		return -PWM;
	else
		return PWM;
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

float DCMotor::getMaxSpeed() const
{
	return m_suppliedVoltage * (2 * M_PI * m_wheelRadius) / (60 * m_speedReductionRatio / m_speedConstant);
}

void DCMotor::updatePins()
{
	if (m_enable && m_speed != 0)
	{
		digitalWrite(m_enablePin, HIGH);
		analogWrite(m_velocityPin, getPWM());
		digitalWrite(m_directionPin, (m_speed > 0) ? FORWARD : BACKWARD);
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
