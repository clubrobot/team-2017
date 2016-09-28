#include <Arduino.h>
#include <FrequencyTimer2.h>
#include <robotcom.h>

#define LED 13

#define SETCOUNT	0x02
#define SETPERIOD	0x03

/* Prototypes */

int setCountCommand(int argc, byte argv[], byte outv[]);
int setPeriodCommand(int argc, byte argv[], byte outv[]);

void toggleLedInterrupt();

/* Global variables */

volatile int count = 0;
volatile int state = LOW;
volatile unsigned long period = 2000000000;

/* Arduino */

void setup()
{
	RobotCom::init();
	RobotCom::addCommand(SETCOUNT, setCountCommand);
	RobotCom::addCommand(SETPERIOD, setPeriodCommand);
	
	pinMode(LED, OUTPUT);
	digitalWrite(LED, state);
	
	FrequencyTimer2::disable();
	FrequencyTimer2::setPeriod(period);
	FrequencyTimer2::setOnOverflow(toggleLedInterrupt);
}

void loop()
{
	RobotCom::executeCommands();
}

/* Commands */

int setCountCommand(int argc, byte argv[], byte outv[])
{
	count += (argc > 1) ? argv[1] : 1;
	return 0;
}

int setPeriodCommand(int argc, byte argv[], byte outv[])
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
	state = !state;
	digitalWrite(LED, state);
}

