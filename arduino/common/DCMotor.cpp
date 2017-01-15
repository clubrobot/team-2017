#include "DCMotor.h"
#include "SerialTalks.h"

#include <Arduino.h>


void DCMotor::setVelocity(float velocity)
{
	m_velocity = velocity;
	update();
}

float DCMotor::getMaximumVelocity() const
{
	return m_suppliedVoltage * (2 * M_PI * m_radius) / (60 * m_reductionRatio / m_velocityConstant);
}

void DCMotor::attach(int EN, int PWM, int DIR)
{
	m_EN  = EN;
	m_PWM = PWM;
	m_DIR = DIR;
	pinMode(m_EN, OUTPUT);
	pinMode(m_PWM, OUTPUT);
	pinMode(m_DIR, OUTPUT);
}

void DCMotor::setRadius(float radius)
{
	m_radius = radius;
}

void DCMotor::setConstants(float velocityConstant, int reductionRatio)
{
	m_velocityConstant = velocityConstant;
	m_reductionRatio = reductionRatio;
}

void DCMotor::setSuppliedVoltage(float suppliedVoltage)
{
	m_suppliedVoltage = suppliedVoltage;
}

void DCMotor::update()
{
	if (m_velocity != 0)
	{
		//talks.out << getPWM() << "\n";
		digitalWrite(m_EN, HIGH);
		analogWrite(m_PWM, getPWM());
		digitalWrite(m_DIR, (m_velocity > 0) ? FORWARD : BACKWARD);
	}
	else
	{
		digitalWrite(m_EN, LOW);
	}
}

int DCMotor::getPWM() const
{
	int PWM = m_velocity * (60 * m_reductionRatio / m_velocityConstant) / (2 * M_PI * m_radius) * (255 / m_suppliedVoltage);
	if (PWM > 255 || PWM < -255)
		return 255;
	else if (PWM < 0)
		return -PWM;
	else
		return PWM;
}

void DCDriver::attach(int RESET, int FAULT)
{
	pinMode(m_RESET, OUTPUT);
	pinMode(m_FAULT, INPUT);
	reset();
}

void DCDriver::reset()
{
	digitalWrite(m_RESET, LOW);
	delayMicroseconds(10); // One may adjust this value.
	digitalWrite(m_RESET, HIGH);
}

bool DCDriver::isFaulty()
{
	return (digitalRead(m_FAULT) == LOW);
}
