#ifndef __VELOCITYCONTROLLER_H__
#define __VELOCITYCONTROLLER_H__

#include "DifferentialController.h"


class VelocityController : public DifferentialController
{
public:

	void setMaximumAccelerations (float linearAcceleration,  float angularAcceleration);
	void setMaximumDeccelerations(float linearDecceleration, float angularDecceleration);

protected:

	virtual void process(float timestep);

	float m_maxLinearAcceleration;
	float m_maxLinearDecceleration;
	float m_maxAngularAcceleration;
	float m_maxAngularDecceleration;
};

#endif // __VELOCITYCONTROLLER_H__
