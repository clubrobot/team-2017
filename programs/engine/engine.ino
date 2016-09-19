#include <Arduino.h>
#include <robotcom.h>

#define CLK			2
#define IN1_2		3
#define PL4			4
#define IN1_1		5
#define EN1			6
#define DATA		7
#define SEL2		8
#define EN2			9
#define IN2_1		10
#define IN2_2		11
#define RSTDRV		12
#define FAULTDRV	A0
#define UDX			A1
#define RSTX		A2
#define RSTY		A3
#define UDY			A4
#define OF			A5
#define SEL1		A6
#define XY			A7
#define SLPDRV		13

void setup()
{
	RobotCom::init();

/*	digitalWrite(RSTDRV, HIGH);
	digitalWrite(SLPDRV, HIGH);//*/
}

void loop()
{
	RobotCom::executeCommands();

/*	int PWM = 64;

	delay(2000);

	digitalWrite(IN1_1, HIGH);
	digitalWrite(IN1_2, LOW);
	analogWrite(EN1, PWM);

	digitalWrite(IN2_1, HIGH);
	digitalWrite(IN2_2, LOW);
	analogWrite(EN2, PWM);

	delay(2000);

	PWM = 192;

	digitalWrite(IN1_1, LOW);
	digitalWrite(IN1_2, HIGH);
	analogWrite(EN1, PWM);

	digitalWrite(IN2_1, LOW);
	digitalWrite(IN2_2, HIGH);
	analogWrite(EN2, PWM);//*/
}
