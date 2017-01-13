#ifndef __DCMOTORSWHEELEDBASE_H__
#define __DCMOTORSWHEELEDBASE_H__

#include "WheeledBase.h"
#include "DCMotor.h"
#include "Odometry.h"
#include "PID.h"


class DCMotorsWheeledBase : public WheeledBase
{
public:

	void setLinearVelocity (float linearVelocity);
	void setAngularVelocity(float angularVelocity);

	void setWheels(DCMotor& leftWheel, DCMotor& rightWheel);
	void setOdometry(Odometry& odometry);

	/* */ PID& getLinearVelocityController () /* */{return m_linearVelocityController;}
	const PID& getLinearVelocityController () const{return m_linearVelocityController;}
	/* */ PID& getAngularVelocityController() /* */{return m_angularVelocityController;}
	const PID& getAngularVelocityController() const{return m_angularVelocityController;}

	void update();

protected:

	DCMotor* m_leftWheel;
	DCMotor* m_rightWheel;

	Odometry* m_odometry;

	float m_linearVelocitySetpoint;
	float m_angularVelocitySetpoint;

	PID m_linearVelocityController;
	PID m_angularVelocityController;
};

#endif // __DCMOTORSBASE_H__
