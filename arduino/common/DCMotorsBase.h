#ifndef __DCMOTORSBASE_H__
#define __DCMOTORSBASE_H__

#include "WheeledBase.h"
#include "DCMotorWheel.h"
#include "Odometry.h"
#include "PID.h"


class DCMotorsBase : public WheeledBase
{
public:

	void setLinearVelocity (float linearVelocity);
	void setAngularVelocity(float angularVelocity);

	void setWheels(DCMotorWheel& left, DCMotorWheel& right);
	void setOdometry(Odometry& odometry);

	/* */ PID& getLinearVelocityController () /* */{return m_linearVelocityController;}
	const PID& getLinearVelocityController () const{return m_linearVelocityController;}
	/* */ PID& getAngularVelocityController() /* */{return m_angularVelocityController;}
	const PID& getAngularVelocityController() const{return m_angularVelocityController;}

	void update();

protected:

	DCMotorWheel* m_leftWheel;
	DCMotorWheel* m_rightWheel;

	Odometry* m_odometry;

	float m_linearVelocitySetpoint;
	float m_angularVelocitySetpoint;

	PID m_linearVelocityController;
	PID m_angularVelocityController;
};

#endif // __DCMOTORSBASE_H__
