#ifndef __DCMOTOR_H__
#define __DCMOTOR_H__

#include <Arduino.h>

#include "NonCopyable.h"


class DCMotor : private NonCopyable
{
public:

	enum DriverMode {FAST_DECAY, SLOW_DECAY};

	DCMotor(int EN, int IN1, int IN2, float wheelRadius, float speedConstant, int speedReductionRatio, int suppliedVoltage);

	int getPWM() const;

	void enable(bool enable = true);
	void setSpeed(float speed);
	void setDriverMode(DriverMode mode);

private:

	void updatePins();

	bool		m_enable;
	float 		m_speed; // in mm/s (millimeters per second)
	DriverMode	m_driverMode;

	const int	m_enablePin;
	const int	m_input1Pin;
	const int	m_input2Pin;

	const float m_wheelRadius; // in mm (millimeters)
	const float	m_speedConstant; // in RPM/V (revolution per minute per volt)
	const int	m_speedReductionRatio;
	const float m_suppliedVoltage; // in V (volt)
};

class DCDriver
{
public:

	DCDriver(int RESET, int FAULT);

	void reset();
	bool isFaulty();

private:

	const int m_resetPin;
	const int m_faultPin;
};

#endif // __DCMOTOR_H__
