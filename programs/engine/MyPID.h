#ifndef __MYPID_H__
#define __MYPID_H__

#include <PID_v1.h>


class MyPID : public PID
{
public:

	MyPID(float Kp, float Ki = 0, float Kd = 0);

	void setInput(float input);
	void setSetpoint(float setpoint);

	float getOutput() const;

private:

	double	m_input;
	double	m_output;
	double	m_setpoint;
};

#endif // __MYPID_H__
