#ifndef __PID_H__
#define __PID_H__

#include "Clock.h"
#include <math.h>


class PID
{
public:

	PID() : m_minOutput(-INFINITY), m_maxOutput(INFINITY){}

	void setTunings(float Kp, float Ki, float Kd);
	void setOutputLimits(float minOutput, float maxOutput);
	void setTimestep(float timestep);

	bool compute(float setpoint, float input, float& output);

	float getKp() const {return m_Kp;}
	float getKi() const {return m_Ki;}
	float getKd() const {return m_Kd;}

	void loadTunings(int address);
	void saveTunings(int address) const;

	void reset();

private:

	float m_Kp;
	float m_Ki;
	float m_Kd;

	float m_output;
	float m_minOutput;
	float m_maxOutput;

	float m_errorIntegral;
	float m_previousError;

	Clock m_clock;
	float m_timestep;
};

#endif // __PID_H__
