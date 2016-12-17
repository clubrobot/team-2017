#ifndef __GRIPPER_H__
#define __GRIPPER_H__

#include <Arduino.h>
#include <Servo.h>

#include "NonCopyable.h"


class Gripper : private NonCopyable
{
public:

	Gripper(int pin, int min, int max, float idlePosition, float holdPosition, float dropPosition);

	void enable();
	void disable();

	void setPosition(float position);

	void idle(){setPosition(m_idlePosition);}
	void hold(){setPosition(m_holdPosition);}
	void drop(){setPosition(m_dropPosition);}

private:

	void updatePins();

	Servo m_servo;

	float m_position; // in range [0, 1] where 0 is `m_min` and 1 is `m_max`

	const int m_pin; // Servo pin
	const int m_min; // in degrees
	const int m_max; // in degrees

	const float m_idlePosition; // See `m_position`
	const float m_holdPosition; // See `m_position`
	const float m_dropPosition; // See `m_position`
};

#endif // __GRIPPER_H__
