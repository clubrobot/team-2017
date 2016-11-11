#include <Arduino.h>
#include <Commands.h>
#include <SerialTalks.h>

#define BLINK	0x02
#define LED		13


SerialTalks com("fabrice");

bool blink(InputStack& input, OutputStack& output)
{
	byte count = 1;
	input >> count;
	for (int i = 0; i < count; i++)
	{
		digitalWrite(LED, HIGH);
		delay(200);
		digitalWrite(LED, LOW);
		delay(200);
	}
	return false;
}

void setup()
{
	com.addInstruction(BLINK, blink);
	pinMode(LED, OUTPUT);
}

void loop()
{
	com.execute();
}
