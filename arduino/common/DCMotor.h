#ifndef __DCMOTOR_H__
#define __DCMOTOR_H__

#include "NonCopyable.h"
#include "DifferentialController.h"


class DCMotor : private NonCopyable, public AbstractMotor
{
public:

	DCMotor() : m_enabled(false), m_velocity(0), m_constant(1){}

	void attach(int EN, int PWM, int DIR);

	void setVelocity(float velocity){m_velocity = velocity; update();}
	void setConstant(float constant){m_constant = constant; update();}
	void enable (){m_enabled = true;  update()}
	void disable(){m_enabled = false; update()}

	float getVelocity() const {return m_velocity;}
	float getConstant() const {return m_constant;}
	bool  isEnabled  () const {return m_enabled;}

	void update();

	void loadConstant(int address);
	void saveConstant(int address) const;
	
protected:

	bool  m_enabled;
	float m_velocity; // in mm/s (millimeters per second)
	float m_constant; // = (60 * reduction_ratio / velocity_constant_in_RPM) / (2 * pi * wheel_radius_in_mm) / supplied_voltage_in_V

	int	m_EN;
	int	m_PWM;
	int	m_DIR;
};

class DCMotorsDriver
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
