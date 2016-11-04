#ifndef __ODOMETRY_H__
#define __ODOMETRY_H__

#include <Arduino.h>

#include "RotaryEncoder.h"
#include "WheeledBase.h"

class Odometry
{
public:

	struct State
	{
		State() : x(0), y(0), theta(0){}
		State(float x, float y, float theta) : x(x), y(y), theta(theta){}

		float x, y;
		float theta;
	};

	struct Movement
	{
		Movement() : dx_dt(0), dy_dt(0), omega(0){}
		Movement(float dx_dt, float dy_dt, float omega) : dx_dt(dx_dt), dy_dt(dy_dt), omega(omega){}

		float dx_dt, dy_dt;
		float omega;
	};

	Odometry(WheeledBase& base);

	const State&	getState() const;
	const Movement&	getMovement() const;

	void setState(float x, float y, float theta);
	void setState(const State& state);

	void setMovement(float dx_dt, float dy_dt, float omega);
	void setMovement(const Movement& movement);

	unsigned long getElapsedTime();

	void update();

private:

	unsigned long m_lastTime;

	State		m_state;
	Movement	m_movement;

	WheeledBase	m_base;
};

#endif // __ODOMETRY_H__
