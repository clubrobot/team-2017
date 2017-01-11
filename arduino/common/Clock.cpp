#include "Clock.h"
#include <Arduino.h>


Clock::Clock()
{
	m_startTime = millis();
}

float Clock::getElapsedTime() const
{
	unsigned long currentTime = millis();
	float elapsedTimeInSeconds = (currentTime - m_startTime) / 1000;
	return elapsedTimeInSeconds;
}

float Clock::restart()
{
	unsigned long currentTime = millis();
	float elapsedTimeInSeconds = (currentTime - m_startTime) / 1000;
	m_startTime = currentTime;
	return elapsedTimeInSeconds;
}
