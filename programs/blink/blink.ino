#include <Arduino.h>
#include <FrequencyTimer2.h>
#include <robotcom.h>

#define LED 13

#define SETCOUNT	0x02
#define SETPERIOD	0x03

/* Proptotypes */

byte setCountCommand(byte argc, byte argv[], byte outv[]);
byte setPeriodCommand(byte argc, byte argv[], byte outv[]);

void toggleLedInterrupt();

/* Global variables */

int count = 0;
unsigned long period = 200000;

/* Arduino */

void setup()
{
	RobotCom::init();
	RobotCom::addCommand(SETCOUNT, setCountCommand);
	RobotCom::addCommand(SETPERIOD, setPeriodCommand);
	
	pinMode(LED, OUTPUT);
	
	FrequencyTimer2::setPeriod(period);
	FrequencyTimer2::setOnOverflow(toggleLedInterrupt);
}

void loop()
{
	RobotCom::executeCommands();
}

/* Commands */

byte setCountCommand(byte argc, byte argv[], byte outv[])
{
	count += (argc > 1) ? argv[1] : 1;
	return 0;
}


byte setPeriodCommand(byte argc, byte argv[], byte outv[])
{
	if (argc > 1 && argv[1] > 0)
	{
		FrequencyTimer2::setPeriod(period);
		FrequencyTimer2::setOnOverflow(toggleLedInterrupt);
	}
	else
	{
		FrequencyTimer2::setOnOverflow(0);
	}
	return 0;
}

/* Interrupts */

void toggleLedInterrupt()
{
	digitalWrite(13, !digitalread(LED));
}

