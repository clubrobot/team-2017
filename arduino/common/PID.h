#ifndef __PID_H__
#define __PID_H__

#include "Clock.h"


class PID
{
public:

	PID(int address);
	PID(int address, float Kp, float Ki, float Kd);

	void setTunings(float Kp, float Ki, float Kd);
	void setOutputLimits(float minOutput, float maxOutput);

	bool compute(float setpoint, float input, float& output);

	float getKp() const {return m_Kp;}
	float getKi() const {return m_Ki;}
	float getKd() const {return m_Kd;}

	void setTimestep(float timestep);

	void reset();

private:

	void loadTunings();
	void saveTunings() const;

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

	const int m_address;
};

#endif // __PID_H__
