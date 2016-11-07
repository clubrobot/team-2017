#include "MyPID.h"
#include <math.h>


MyPID::MyPID(float Kp, float Ki, float Kd)
:	PID(&m_input, &m_output, &m_setpoint, Kp, Ki, Kd, DIRECT)
,	m_input(0)
,	m_output(0)
,	m_setpoint(0)
{
	SetOutputLimits(-INFINITY, INFINITY);
	SetMode(AUTOMATIC);
}

void MyPID::setInput(float input)
{
	m_input = input;
}

void MyPID::setSetpoint(float setpoint)
{
	m_setpoint = setpoint;
}

float MyPID::getOutput() const
{
	return m_output;
}
