#include <Arduino.h>
#include <SerialTalks.h>

#define BLINK	0x02
#define LED		13


bool blink(Deserializer& input, Serializer& output)
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
	talks.begin(Serial);
	talks.attach(BLINK, blink);
	pinMode(LED, OUTPUT);

	char uuid[SERIALTALKS_UUID_LENGTH];
	talks.getUUID(uuid);
	talks.out << "Hello there, my name is " << uuid << "!\n";
	talks.err << "Something happened :(\n";
}

void loop()
{
	static int i = 0;
	talks.execute();
	talks.out << i++ << "\n";
	delay(100);
}
