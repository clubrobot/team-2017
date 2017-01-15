#include "hardware.h"
#include "SerialTalks.h"


byte readShiftRegister(int Q, int PL, int CP)
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
