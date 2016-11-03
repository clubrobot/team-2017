#include "RotaryEncoder.h"
#include <math.h>

#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif


RotaryEncoder::RotaryEncoder(Axis axis, int XY, int SEL1, int SEL2, int OE, int RST, int Q, int PL, int CP, long countsPerRevolution, float wheelRadius)
:	m_counterAxis(axis)
,	m_counterValue(0)

,	m_counterAxisPin(XY)
,	m_counterByteSelect1Pin(SEL1)
,	m_counterByteSelect2Pin(SEL2)
,	m_counterOutputEnablePin(OE)
,	m_counterResetPin(RST)

,	m_registerDataPin(Q)
,	m_registerLatchPin(PL)
,	m_registerShiftPin(CP)

,	m_countsPerRevolution(countsPerRevolution)
,	m_wheelRadius(wheelRadius)
{
	pinMode(m_counterAxisPin, OUTPUT);
	pinMode(m_counterByteSelect1Pin, OUTPUT);
	pinMode(m_counterByteSelect2Pin, OUTPUT);
	pinMode(m_counterOutputEnablePin, OUTPUT);
	pinMode(m_counterResetPin, OUTPUT);

	digitalWrite(m_counterOutputEnablePin, HIGH);
	resetCounter();

	pinMode(m_registerDataPin, INPUT);
	pinMode(m_registerLatchPin, OUTPUT);
	pinMode(m_registerShiftPin, OUTPUT);

	digitalWrite(m_registerLatchPin, LOW);
	digitalWrite(m_registerShiftPin, LOW);
}

void RotaryEncoder::updateCounter()
{
	digitalWrite(m_counterAxisPin, m_counterAxis);
	digitalWrite(m_counterOutputEnablePin, LOW);
	m_counterValue = 0;
	for (int i = 0; i < 4; i++)
	{
		delayMicroseconds(100); // TODO: is this necessary? Can we reduce it?
		int sel1 = i & 0x01;
		int sel2 = (~i >> 1) & 0x01;
		digitalWrite(m_counterByteSelect1Pin, sel1);
		digitalWrite(m_counterByteSelect2Pin, sel2);
		m_counterValue <<= 8;
		m_counterValue += readShiftRegister(m_registerDataPin, m_registerLatchPin, m_registerShiftPin);
	}
	digitalWrite(m_counterOutputEnablePin, HIGH);
}

void RotaryEncoder::resetCounter()
{
	digitalWrite(m_counterResetPin, LOW);
	digitalWrite(m_counterResetPin, HIGH);
}

long RotaryEncoder::getCounter() const
{
	return m_counterValue;
}

float RotaryEncoder::getTraveledDistance()
{
	long oldCounter = getCounter();
	updateCounter();
	long counts = getCounter() - oldCounter;
	
	float distance = counts / m_countsPerRevolution * 2.0 * M_PI * m_wheelRadius;
	return distance;
}

byte RotaryEncoder::readShiftRegister(int Q, int PL, int CP)
{
	digitalWrite(PL, HIGH);
	byte value = 0;
	for (int i = 0; i < 8; i++)
	{
		value <<= 1;
		value |= digitalRead(Q);
		digitalWrite(CP, HIGH);
		digitalWrite(CP, LOW);
	}
	digitalWrite(PL, LOW);
	return value;
}
