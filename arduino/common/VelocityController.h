#ifndef __VELOCITYCONTROLLER_H__
#define __VELOCITYCONTROLLER_H__

#include "DifferentialController.h"

#include <math.h>


class VelocityController : public DifferentialController
{
public:

	VelocityController() : m_rampLinVelSetpoint(0), m_rampAngVelSetpoint(0), m_maxLinAcc(INFINITY), m_maxLinDec(INFINITY), m_maxAngAcc(INFINITY), m_maxAngDec(INFINITY){}

	void setMaxAcc(float maxLinAcc, float maxAngAcc){m_maxLinAcc = maxLinAcc; m_maxAngAcc = maxAngAcc;}
	void setMaxDec(float maxLinDec, float maxAngDec){m_maxLinDec = maxLinDec; m_maxAngDec = maxAngDec;}

	float getMaxLinAcc() const {return m_maxLinAcc;}
	float getMaxAngAcc() const {return m_maxAngAcc;}
	float getMaxLinDec() const {return m_maxLinDec;}
	float getMaxAngDec() const {return m_maxAngDec;}

	void load(int address);
	void save(int address) const;

protected:

	float genRampSetpoint(float stepSetpoint, float input, float rampSetpoint, float maxAcc, float maxDec, float timestep);

	virtual void process(float timestep);
	virtual void onProcessEnabling();

	float m_rampLinVelSetpoint; // in mm/s (no longer w/e unit)
	float m_rampAngVelSetpoint; // in rad/s (no longer w/e unit)
	float m_maxLinAcc; // always positive, in mm/s^2
	float m_maxLinDec; // always positive, in mm/s^2
	float m_maxAngAcc; // always positive, in rad/s^2
	float m_maxAngDec; // always positive, in rad/s^2
};

#endif // __VELOCITYCONTROLLER_H__
