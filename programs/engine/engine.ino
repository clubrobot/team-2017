#include <Arduino.h>
#include <robotcom.h>

//*************************************************************************
//								PINOUT									  *
//*************************************************************************

// Motor control
#define RSTDRV		12		// Reset motor driver. Logic low to reset
#define SLPDRV		13		// Driver sleep mode. Logic low to enter sleep mode
#define FAULTDRV	A0		// Fault condition driver output
// Moteur 1 = Moteur gauche
#define EN1			6		// ENable MOTOR 1 DRV8844
#define IN1_1		5		// IN1 DRV8844 (MOTOR 1)
#define IN1_2		3		// IN2 DRV8844 (MOTOR 1)
// Moteur 2 = Moteur droit
#define EN2			9		// ENable MOTOR 2 DRV8844
#define IN2_1		10		// IN3 DRV8844 (MOTOR 2)
#define IN2_2		11		// IN4 DRV8844 (MOTOR 2)
//74HC165 register
#define CLK			2		// clock signal for 74HC165 register
#define DATA		7		// Serial data input from 74HC165 register
#define PL			4		// PL for 74HC165 register
// Quadrature decoder HCTL-2032
#define RSTX		A2		// Active LOW. Clears the internal position counter and the position latch.
#define RSTY		A3		// Active LOW. Clears the internal position counter and the position latch.
// Counter byte selection
// 		| MSB | 2ND | 3RD | LSB |
// SEL1 |  0  |  1  |  0  |  1  |
// SEL2 |  1  |  1  |  0  |  0  |
#define SEL1		A6		 
#define SEL2		8		
#define OE			A5		// Logic LOW to enable the tri-states output buffers
#define XY			A7		// Select the 1st or 2ND axis data to be read. LOW = X axis HIGH = Y axis
// Signal informations
#define UDX			A1		// Not used (must be used with CNTDEC and CNTCAS)
#define UDY			A4		// Not used (must be used with CNTDEC and CNTCAS)




void setup()
{
	RobotCom::init();

/*	digitalWrite(RSTDRV, HIGH);
	digitalWrite(SLPDRV, HIGH);//*/


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
	analogWrite(EN2, PWM);*/
}
