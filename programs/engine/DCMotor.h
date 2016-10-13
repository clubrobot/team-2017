#ifndef __DCMOTOR_H__
#define __DCMOTOR_H__

#include <Arduino.h>

class DCMotor
{
public:

	enum DriverMode {FAST_DECAY, SLOW_DECAY};

	DCMotor(int enablePin, int input1Pin, int input2Pin);

	void enable(bool enable = true);
	void setSpeed(float ratio);
	void setDirection(Direction direction);
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

#endif // __DCMOTOR_H__
