#ifndef __DCMOTORSWHEELEDBASE_H__
#define __DCMOTORSWHEELEDBASE_H__

#include "WheeledBase.h"
#include "DCMotor.h"
#include "Odometry.h"
#include "PID.h"
#include "Clock.h"

#define OUTPUT_CONTROL_VARIABLES 1 // Debug
#define CONTROL_IN_POSITION 0 // Experimental


class DCMotorsWheeledBase : public WheeledBase
{
public:

	virtual void setLinearVelocity (float linearVelocity);
	virtual void setAngularVelocity(float angularVelocity);

	void setWheels(DCMotor& leftWheel, DCMotor& rightWheel);
	void setOdometry(Odometry& odometry);
#if CONTROL_IN_POSITION
	void setPIDControllers(PID& linear, PID& angular);
#else
	void setPIDControllers(PID& linear, PID& angular);
#endif

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

#if CONTROL_IN_POSITION
	float m_traveledDistanceSetpoint;
	float m_thetaSetpoint;

	Clock m_clock;

	PID* m_linearPositionController;
	PID* m_angularPositionController;
#else
	PID* m_linearVelocityController;
	PID* m_angularVelocityController;
#endif
};

#endif // __DCMOTORSBASE_H__
