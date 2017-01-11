#include "Clock.h"
#include <Arduino.h>


Clock::Clock()
{
	m_startTime = micros();
}

float Clock::getElapsedTime() const
{
	unsigned long currentTime = micros();
	float elapsedTimeInSeconds = (currentTime - m_startTime) / float(1e6);
	return elapsedTimeInSeconds;
}

float Clock::restart()
{
	unsigned long currentTime = micros();
	float elapsedTimeInSeconds = (currentTime - m_startTime) / float(1e6);
	m_startTime = currentTime;
	return elapsedTimeInSeconds;
}
