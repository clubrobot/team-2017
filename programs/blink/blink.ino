#include <Arduino.h>
#include <robotcom.h>


void setup()
{
	RobotCom::init();
}

void loop()
{
	RobotCom::executeCommands();
}
