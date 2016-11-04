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
		Movement() : dx(0), dy(0), omega(0){}
		Movement(float dx, float dy, float omega) : dx(dx), dy(dy), omega(omega){}

		float dx, dy;
		float omega;
	};

	Odometry(WheeledBase& base);

	const State&	getState() const;
	const Movement&	getMovement() const;

	void setState(float x, float y, float theta);
	void setState(const State& state);

	void setMovement(float dx, float dy, float omega);
	void setMovement(const Movement& movement);

	void update();

private:

	State		m_state;
	Movement	m_movement;

	WheeledBase	m_base;
};

#endif // __ODOMETRY_H__
