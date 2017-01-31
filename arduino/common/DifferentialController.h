#ifndef __DIFFERENTIALCONTROLLER_H__
#define __DIFFERENTIALCONTROLLER_H__

#include "PeriodicProcess.h"
#include "PID.h"

#define OUTPUT_CONTROL_VARIABLES 0 // Debug


class AbstractMotor
{
public:

	virtual ~AbstractMotor(){}

	virtual void setVelocity(float velocity) = 0;
};

class DifferentialController : public PeriodicProcess
{
public:

	void setInputs(float linearInput, float angularInput);
	void setSetpoints(float linearSetpoint, float angularSetpoint);

	void setAxleTrack(float axleTrack);
	void setWheels(AbstractMotor& leftWheel, AbstractMotor& rightWheel);
	void setControllers(PID& linearController, PID& angularController);

private:

	virtual void process(float timestep);
	virtual void onProcessEnabling();

	float m_linearInput;
	float m_linearSetpoint;
	float m_angularInput;
	float m_angularSetpoint;

	float m_axleTrack;
	AbstractMotor* m_leftWheel;
	AbstractMotor* m_rightWheel;
	PID* m_linearController;
	PID* m_angularController;
};

#endif // __DIFFERENTIALCONTROLLER_H__
