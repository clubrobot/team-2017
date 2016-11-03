#ifndef __ROTARYENCODER_H__
#define __ROTARYENCODER_H__

#include <Arduino.h>

class RotaryEncoder
{
public:

	enum Axis {X = 0, Y = 1};

	RotaryEncoder(Axis axis, int XY, int SEL1, int SEL2, int OE, int RST, int Q, int PL, int CP, long countsPerRevolution, float wheelRadius);

	void updateCounter();
	void resetCounter();
	long getCounter() const;

	static byte readShiftRegister(int Q, int PL, int CP);

	float getTraveledDistance();

private:

	Axis m_counterAxis;
	long m_counterValue;

	const int m_counterAxisPin;			// X = 0, Y = 1
	const int m_counterByteSelect1Pin;	// MSB = 0, 2ND = 1, 3RD = 0, LSB = 1
	const int m_counterByteSelect2Pin;	// MSB = 0, 2ND = 0, 3RD = 1, LSB = 1
	const int m_counterOutputEnablePin;	// Active LOW. Enable the tri-states output buffers.
	const int m_counterResetPin;		// Active LOW. Clear the internal position counter and the position latch.

	const int m_registerDataPin;		// Serial data input from the 74HC165 register.
	const int m_registerLatchPin;		// Active LOW. Latch signal for the 74HC165 register.
	const int m_registerShiftPin;		// LOW-to-HIGH edge-triggered. Clock signal for the 74HC165 register.

	const long	m_countsPerRevolution;
	const float m_wheelRadius;
};

#endif // __ROTARYENCODER_H__
