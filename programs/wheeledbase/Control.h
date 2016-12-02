#ifndef __CONTROL_H__
#define __CONTROL_H__

#include <Arduino.h>

#include "WheeledBase.h"
#include "Odometry.h"
#include "MyPID.h"


class Control
{
public:

	Control(WheeledBase& base, Odometry& odometry);

	void setVelocitySetpoint(float setpoint);
	void setOmegaSetpoint(float setpoint);

	void step();

	void enable();
	void disable();

private:

	MyPID			m_velocityController;
	MyPID			m_omegaController;

	WheeledBase&	m_base;
	Odometry&		m_odometry;

	bool			m_enabled;
};

#endif //__CONTROL_H__
