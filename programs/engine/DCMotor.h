#ifndef __DCMOTOR_H__
#define __DCMOTOR_H__

#include <Arduino.h>

class DCMotor
{
public:

	enum DriverMode {FAST_DECAY, SLOW_DECAY};

	DCMotor(int EN, int IN1, int IN2);

	void enable(bool enable = true);
	void setSpeed(float ratio);
	void setDriverMode(DriverMode mode);

private:

	void updatePins();

	bool		m_enable;
	float 		m_ratio;
	DriverMode	m_driverMode;

	const int	m_enablePin;
	const int	m_input1Pin;
	const int	m_input2Pin;
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
