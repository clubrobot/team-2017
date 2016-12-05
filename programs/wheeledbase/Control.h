#ifndef __CONTROL_H__
#define __CONTROL_H__

#include <Arduino.h>

#include "WheeledBase.h"
#include "Odometry.h"
#include "MyPID.h"

#define LINEAR_VELOCITY_PID_ADDRESS  0x040
#define ANGULAR_VELOCITY_PID_ADDRESS 0x060


class Control
{
public:

	Control(WheeledBase& base, Odometry& odometry);

	const MyPID& getLinearVelocityPID () const {return m_linearVelocityPID;}
	const MyPID& getAngularVelocityPID() const {return m_angularVelocityPID;}

	void setLinearVelocity (float setpoint){m_linearVelocityPID .setSetpoint(setpoint);}
	void setAngularVelocity(float setpoint){m_angularVelocityPID.setSetpoint(setpoint);}

	void setLinearVelocityPIDTunings(float Kp, float Ki, float Kd)
	{
		m_linearVelocityPID.setTunings(Kp, Ki, Kd);
	}
	void setAngularVelocityPIDTunings(float Kp, float Ki, float Kd)
	{
		m_angularVelocityPID.setTunings(Kp, Ki, Kd);
	}

	void step();

	void enable();
	void disable();

private:

	MyPID			m_linearVelocityPID;
	MyPID			m_angularVelocityPID;

	WheeledBase&	m_base;
	Odometry&		m_odometry;

	bool			m_enabled;
};

#endif //__CONTROL_H__
