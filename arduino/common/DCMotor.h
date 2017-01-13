#ifndef __DCMOTOR_H__
#define __DCMOTOR_H__

#include "NonCopyable.h"

#define FORWARD  0
#define BACKWARD 1


class DCMotor : private NonCopyable
{
public:

	void setVelocity(float velocity);

	float getMaximumVelocity() const;

	void enable();
	void disable();

	void attach(int EN, int PWM, int DIR);

	void setRadius(float radius);
	void setConstants(float velocityConstant, int reductionRatio);
	void setSuppliedVoltage(float suppliedVoltage);

	void update();
	
protected:

	int getPWM() const;

	bool  m_enabled;
	float m_velocity; // in mm/s (millimeters per second)

	int	m_EN;
	int	m_PWM;
	int	m_DIR;

	float m_radius; // in mm (millimeters)
	float m_velocityConstant; // in RPM/V (revolution per minute per volt)
	int	  m_reductionRatio;
	float m_suppliedVoltage; // in V (volt)
};

class DCDriver
{
public:

	void attach(int RESET, int FAULT);

	void reset();
	bool isFaulty();

private:

	int m_RESET;
	int m_FAULT;
};

#endif // __DCMOTOR_H__
