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

	enum Target{VELOCITIES, STANCE};

	Control(WheeledBase& base, Odometry& odometry);

	const MyPID& getLinearVelocityPID () const {return m_linearVelocityPID;}
	const MyPID& getAngularVelocityPID() const {return m_angularVelocityPID;}

	void setLinearVelocityPIDTunings(float Kp, float Ki, float Kd)
	{
		m_linearVelocityPID.setTunings(Kp, Ki, Kd);
	}
	void setAngularVelocityPIDTunings(float Kp, float Ki, float Kd)
	{
		m_angularVelocityPID.setTunings(Kp, Ki, Kd);
	}

	void setTargetVelocities(float linear, float angular);
	void setTargetStance(float x, float y, float theta, float linear, float angular);

	void enable();
	void disable();

	void step();

	void reset();

private:

	void setVelocitiesSetpointsDependingOnTargetVelocities();
	void setVelocitiesSetpointsDependingOnTargetStance();

	MyPID        m_linearVelocityPID;
	MyPID        m_angularVelocityPID;

	Movement     m_targetVelocities;
	State        m_targetStance;
	Target       m_target;

	WheeledBase& m_base;
	Odometry&    m_odometry;

	bool         m_enabled;
};

#endif //__CONTROL_H__
