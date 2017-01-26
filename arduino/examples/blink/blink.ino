#include <Arduino.h>

#include "../../common/SerialTalks.h"

#define BLINK	0x0B
#define ECHO    0x0E
#define LED		13


void blink(SerialTalks& inst, Deserializer& input, Serializer& output)
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
}

void echo(SerialTalks& inst, Deserializer& input, Serializer& output)
{
	char buffer[SERIALTALKS_INPUT_BUFFER_SIZE];
	input >> buffer;
	output << buffer;
}

void setup()
{
	Serial.begin(SERIALTALKS_BAUDRATE);
	talks.begin(Serial);
	talks.bind(BLINK, blink);
	talks.bind(ECHO, echo);
	pinMode(LED, OUTPUT);

	talks.waitUntilConnected();

	char uuid[SERIALTALKS_UUID_LENGTH];
	talks.getUUID(uuid);
	talks.out << "Hello there, my name is " << uuid << "!\n";
	talks.err << "Something happened :(\n";//*/
}

void loop()
{
	talks.execute();

	static int i = 0;
	talks.out << i++ << "\n";
	delay(100);//*/
}
