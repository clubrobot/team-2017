#ifndef __DCMOTORSWHEELEDBASE_H__
#define __DCMOTORSWHEELEDBASE_H__

#include "WheeledBase.h"
#include "DCMotor.h"
#include "Odometry.h"
#include "PID.h"


class DCMotorsWheeledBase : public WheeledBase
{
public:

	virtual void setLinearVelocity (float linearVelocity);
	virtual void setAngularVelocity(float angularVelocity);

	void setWheels(DCMotor& leftWheel, DCMotor& rightWheel);
	void setOdometry(Odometry& odometry);
	void setPIDControllers(PID& linear, PID& angular);

	void enable();
	void disable();

	void update();

protected:

	bool m_enabled;

	DCMotor* m_leftWheel;
	DCMotor* m_rightWheel;

	Odometry* m_odometry;

	float m_linearVelocitySetpoint;
	float m_angularVelocitySetpoint;

	PID* m_linearVelocityController;
	PID* m_angularVelocityController;
};

#endif // __DCMOTORSBASE_H__
