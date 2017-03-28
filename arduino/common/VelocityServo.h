#ifndef __VELOCITYSERVO_H__
#define __VELOCITYSERVO_H__

#include "Clock.h"
#include <Servo.h>
#include "PeriodicProcess.h"

class VelocityServo : public PeriodicProcess{

private : 

	Servo m_servo;
	int m_setpoint;
	int m_velocity;
	int m_startpoint;
	int m_dir;
	bool m_arrived; 
	Clock m_clock;

public :

	void attach(int pin);
	void detach();
	void process(float timestep);
	void Velocitywrite(int vel, int setpoint);
	void write(int setpoint){m_servo.write(setpoint);}
	bool attached(){return m_servo.attached();}
	int read(){return m_servo.read();}
	
};

#endif