#ifndef __VELOCITYCONTROLLER_H__
#define __VELOCITYCONTROLLER_H__

#include "DifferentialController.h"


class VelocityController : public DifferentialController
{
public:

	VelocityController() : m_transitionalLinearVelocitySetpoint(0), m_transitionalAngularVelocitySetpoint(0){}

	void setMaximumAccelerations (float linearAcceleration,  float angularAcceleration);
	void setMaximumDeccelerations(float linearDecceleration, float angularDecceleration);

protected:

	float generateRampSetpoint(float stepSetpoint, float input, float rampSetpoint, float acceleration, float decceleration, float timestep);

	virtual void process(float timestep);

	virtual void onProcessEnabling();

	float m_transitionalLinearVelocitySetpoint;
	float m_transitionalAngularVelocitySetpoint;

	float m_maxLinearAcceleration;
	float m_maxLinearDecceleration;
	float m_maxAngularAcceleration;
	float m_maxAngularDecceleration;
};

#endif // __VELOCITYCONTROLLER_H__
