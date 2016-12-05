#ifndef __ODOMETRY_H__
#define __ODOMETRY_H__

#include <Arduino.h>

#include "RotaryEncoder.h"
#include "WheeledBase.h"


struct State
{
	State() : x(0), y(0), theta(0){}
	State(float x, float y, float theta) : x(x), y(y), theta(theta){}

	float x, y;
	float theta;
};

struct Movement
{
	Movement() : linear(0), angular(0){}
	Movement(float linear, float angular) : linear(linear), angular(angular){}

	float linear;
	float angular;
};

class Odometry
{
public:

	Odometry(WheeledBase& base);

	const State&	getState() const;
	const Movement&	getMovement() const;

	void setState(float x, float y, float theta);
	void setState(const State& state);

	void setMovement(float linear, float angular);
	void setMovement(const Movement& movement);

	unsigned long getElapsedTime();

	void update();

private:

	unsigned long	m_lastTime;

	State			m_state;
	Movement		m_movement;

	WheeledBase&	m_base;
};

#endif // __ODOMETRY_H__
