#include "SerialTalks.h"
#include "Gripper.h"


Gripper::Gripper(int pin, int min, int max, float idlePosition, float holdPosition, float dropPosition)
:	m_pin(pin)
,	m_min(min)
,	m_max(max)

,	m_idlePosition(idlePosition)
,	m_holdPosition(holdPosition)
,	m_dropPosition(dropPosition)
{
	setPosition(0.5);
}

void Gripper::enable()
{
	m_servo.attach(m_pin);
	updatePins();
}

void Gripper::disable()
{
	m_servo.detach();
}

void Gripper::setPosition(float position)
{
	m_position = position;
	updatePins();
}

void Gripper::updatePins()
{
	int positionInDegrees;
	if (m_position < 0)
		positionInDegrees = m_min;
	else if (m_position > 1)
		positionInDegrees = m_max;
	else
		positionInDegrees = m_min + (m_max - m_min) * m_position;
	m_servo.write(positionInDegrees);
}
