#ifndef __CONTROL_H__
#define __CONTROL_H__

#include <Arduino.h>

#include "WheeledBase.h"
#include "Odometry.h"
#include "MyPID.h"

#define VELOCITY_CONTROLLER_ADDRESS 0x040
#define OMEGA_CONTROLLER_ADDRESS    0x060


class Control
{
public:

	Control(WheeledBase& base, Odometry& odometry);

	const MyPID& getVelocityController() const {return m_velocityController;}
	const MyPID& getOmegaController   () const {return m_omegaController;}

	void setVelocitySetpoint(float setpoint){m_velocityController.setSetpoint(setpoint);}
	void setOmegaSetpoint   (float setpoint){m_omegaController   .setSetpoint(setpoint);}

	void setVelocityControllerTunings(float Kp, float Ki, float Kd)
	{
		m_velocityController.setTunings(Kp, Ki, Kd);
	}
	void setOmegaControllerTunings(float Kp, float Ki, float Kd)
	{
		m_omegaController.setTunings(Kp, Ki, Kd);
	}

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
