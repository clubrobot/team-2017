#ifndef __ODOMETRY_H__
#define __ODOMETRY_H__

#include <Arduino.h>

#include "RotaryEncoder.h"

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

	Odometry(RotaryEncoder& leftEncoder, RotaryEncoder& rightEncoder, float axleTrack);

	const State& getState() const;

	void setState(float x, float y, float theta);
	void setState(const State& state);

	void integrate();

private:

	State m_state;
	RotaryEncoder& m_leftEncoder;
	RotaryEncoder& m_rightEncoder;

	const float m_axleTrack;
};

#endif // __ODOMETRY_H__
