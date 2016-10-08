#include <Arduino.h>
#include <robotcom.h>

#define LED 13

#define BLINK 0x02

/* Prototypes */

int blinkCommand(int argc, byte argv[], byte outv[]);

/* Arduino */

void setup()
{
	RobotCom::init();
	RobotCom::addCommand(BLINK, blinkCommand);
	
	pinMode(LED, OUTPUT);
}

void loop()
{
	RobotCom::executeCommands();
}

/* Commands */

int blinkCommand(int argc, byte argv[], byte outv[])
{
	int count = (argc > 1) ? argv[1] : 1;
	for (int i = 0; i < count; i++)
	{
		digitalWrite(LED, HIGH);
		delay(200);
		digitalWrite(LED, LOW);
		delay(200);
	}
	return 0;
}
