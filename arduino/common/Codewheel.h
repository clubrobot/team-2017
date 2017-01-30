#ifndef __CODEWHEEL_H__
#define __CODEWHEEL_H__

#include "NonCopyable.h"
#include "Odometry.h"

#define X 0
#define Y 1


class Codewheel : private NonCopyable, public AbstractCodewheel
{
public:

	float getTraveledDistance();
	float restart();

	void attachCounter(int XY, int SEL1, int SEL2, int OE, int RST);
	void attachRegister(int DATA, int LATCH, int CLOCK);

	void setAxis(int axis);
	void setRadius(float radius);
	void setCountsPerRevolution(long counts);

	void reset();
	void update();

private:

	long m_currentCounter;
	long m_startCounter;

	int m_COUNTER_XY;   // X = 0, Y = 1
	int m_COUNTER_SEL1; // MSB = 0, 2ND = 1, 3RD = 0, LSB = 1
	int m_COUNTER_SEL2; // MSB = 0, 2ND = 0, 3RD = 1, LSB = 1
	int m_COUNTER_OE;   // Active LOW. Enable the tri-states output buffers.
	int m_COUNTER_RST;  // Active LOW. Clear the internal position counter and the position latch.

	int m_REGISTER_DATA;  // Serial data input from the 74HC165 register.
	int m_REGISTER_LATCH; // Active LOW. Latch signal for the 74HC165 register.
	int m_REGISTER_CLOCK; // LOW-to-HIGH edge-triggered. Clock signal for the 74HC165 register.

	int   m_axis;
	float m_radius;
	long  m_countsPerRevolution;
};

#endif // __ROTARYENCODER_H__
