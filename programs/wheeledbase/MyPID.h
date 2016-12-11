#ifndef __MYPID_H__
#define __MYPID_H__

#include <PID_v1.h>


class MyPID : private PID
{
public:

	MyPID(const int address);

	void setInput   (float input   ){m_input    = input;}
	void setSetpoint(float setpoint){m_setpoint = setpoint;}
	void setTunings (float Kp, float Ki, float Kd);

	float getOutput() const {return m_output;}
	float getKp    () const {return ((PID*)this)->GetKp();}
	float getKi    () const {return ((PID*)this)->GetKi();}
	float getKd    () const {return ((PID*)this)->GetKd();}

	bool step(){return PID::Compute();}

	void enable (){PID::SetMode(AUTOMATIC);}
	void disable(){PID::SetMode(MANUAL);}

	void reset();

private:

	void loadTunings();
	void saveTunings() const;

	double    m_input;
	double    m_output;
	double    m_setpoint;

	const int m_address;
};

#endif // __MYPID_H__
