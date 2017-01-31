#ifndef __PID_H__
#define __PID_H__

#include <math.h>


class PID
{
public:

	PID() : m_minOutput(-INFINITY), m_maxOutput(INFINITY){}

	void setTunings(float Kp, float Ki, float Kd);
	void setOutputLimits(float minOutput, float maxOutput);

	float compute(float setpoint, float input, float timestep);
	void reset();

	float getKp() const {return m_Kp;}
	float getKi() const {return m_Ki;}
	float getKd() const {return m_Kd;}

	void loadTunings(int address);
	void saveTunings(int address) const;

private:

	float m_Kp;
	float m_Ki;
	float m_Kd;

	float m_minOutput;
	float m_maxOutput;

	float m_errorIntegral;
	float m_previousError;
};

#endif // __PID_H__
