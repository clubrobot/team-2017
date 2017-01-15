#include "Codewheel.h"
#include "SerialTalks.h"
#include <math.h>
#include "hardware.h"


float Codewheel::getTraveledDistance()
{
	return (float)(m_currentCounter - m_startCounter) / m_countsPerRevolution * 2.0 * M_PI * m_radius;
}

float Codewheel::restart()
{
	float distance = getTraveledDistance();
	m_startCounter = m_currentCounter;
	return distance;
}

void Codewheel::attachCounter(int XY, int SEL1, int SEL2, int OE, int RST)
{
	m_COUNTER_XY   = XY;
	m_COUNTER_SEL1 = SEL1;
	m_COUNTER_SEL2 = SEL2;
	m_COUNTER_OE   = OE;
	m_COUNTER_RST  = RST;
	pinMode(m_COUNTER_XY  , OUTPUT);
	pinMode(m_COUNTER_SEL1, OUTPUT);
	pinMode(m_COUNTER_SEL2, OUTPUT);
	pinMode(m_COUNTER_OE  , OUTPUT);
	pinMode(m_COUNTER_RST , OUTPUT);
}

void Codewheel::attachRegister(int DATA, int LATCH, int CLOCK)
{
	m_REGISTER_DATA  = DATA;
	m_REGISTER_LATCH = LATCH;
	m_REGISTER_CLOCK = CLOCK;
	pinMode(m_REGISTER_DATA , INPUT);
	pinMode(m_REGISTER_LATCH, OUTPUT);
	pinMode(m_REGISTER_CLOCK, OUTPUT);
}

void Codewheel::setAxis(int axis)
{
	m_axis = axis;
}

void Codewheel::setRadius(float radius)
{
	m_radius = radius;
}

void Codewheel::setCountsPerRevolution(long countsPerRevolution)
{
	m_countsPerRevolution = countsPerRevolution;
}

void Codewheel::reset()
{
	digitalWrite(m_COUNTER_RST, LOW);
	digitalWrite(m_COUNTER_RST, HIGH);
}

void Codewheel::update()
{
	digitalWrite(m_COUNTER_XY, m_axis);
	digitalWrite(m_COUNTER_OE, LOW);
	m_currentCounter = 0;
	for (int i = 0; i < 4; i++)
	{
		delayMicroseconds(100); // TODO: is this necessary? Can we reduce it?
		byte SEL1 = i & 0x01;
		byte SEL2 = (~i >> 1) & 0x01;
		digitalWrite(m_COUNTER_SEL1, SEL1);
		digitalWrite(m_COUNTER_SEL2, SEL2);
		m_currentCounter <<= 8;
		m_currentCounter += readShiftRegister(m_REGISTER_DATA, m_REGISTER_LATCH, m_REGISTER_CLOCK);
	}
	digitalWrite(m_COUNTER_OE, HIGH);
}
