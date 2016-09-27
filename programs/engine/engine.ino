#include <Arduino.h>
#include <robotcom.h>

byte shiftInFixed(byte dataPin, byte clockPin);
void updateCounter(byte xy);

//*************************************************************************
//								PINOUT									  *
//*************************************************************************

// Motor control
#define RSTDRV		12		// Reset motor driver. Logic low to reset
#define FAULTDRV	A7		// Fault condition driver output
// Moteur 1 = Moteur gauche
#define EN1			6		// ENable MOTOR 1 DRV8844
#define IN1_1		5		// IN1 DRV8844 (MOTOR 1)
#define IN1_2		3		// IN2 DRV8844 (MOTOR 1)
// Moteur 2 = Moteur droit
#define EN2			9		// ENable MOTOR 2 DRV8844
#define IN2_1		10		// IN3 DRV8844 (MOTOR 2)
#define IN2_2		11		// IN4 DRV8844 (MOTOR 2)
//74HC165 shift in register
#define CLK			2		// clock signal for 74HC165 register
#define DATA		13	// Serial data input from 74HC165 register
#define LATCH		4		// LATCH of the 74HC165 register (PL)
// Quadrature decoder HCTL-2032
#define RSTX		A2		// Active LOW. Clears the internal position counter and the position latch.
#define RSTY		A3		// Active LOW. Clears the internal position counter and the position latch.
// Counter byte selection
// 		| MSB | 2ND | 3RD | LSB |
// SEL1 |  0  |  1  |  0  |  1  |
// SEL2 |  1  |  1  |  0  |  0  |
#define SEL1		7
#define SEL2		8
#define OE			A5		// Logic LOW to enable the tri-states output buffers
#define XY			A0		// Select the 1st or 2ND axis data to be read. LOW = right HIGH = left
// Signal informations
#define UDX			A1		// Not used (must be used with CNTDEC and CNTCAS)
#define UDY			A4		// Not used (must be used with CNTDEC and CNTCAS)



long rightCounter = 0;
long leftCounter = 0;



//*************************************************************************
//								SETUP									  *
//*************************************************************************
/*
  int getCodedWheel(int argc, byte argv[], byte outv [])
  {
	long& counter = (argv[1] == LOW) ? rightCounter : leftCounter;
	for (int i = 0; i<4;i++)
	{
		outv[i+1] = counter<<(i*8) & 0xFF;
	}

	return 5;
  }
*/

int moteurDroitPWM(int argc, byte argv[], byte outv[])
{
  	analogWrite(EN2, (argc>2)?argv[1]:0);
    if(argc>2)
    {
      digitalWrite(IN2_1, !argv[2]);
    	digitalWrite(IN2_2, argv[2]);
    }
    return 0;
}
int moteurGauchePWM(int argc, byte argv[], byte outv[])
{
  	analogWrite(EN1, (argc>2)?argv[1]:0);
    if(argc>2)
    {
      digitalWrite(IN1_1, !argv[2]);
    	digitalWrite(IN1_2, argv[2]);
    }
    return 0;
}

void setup()
{
  RobotCom::init();
  //RobotCom::addCommand(0x02, getCodedWheel);
  RobotCom::addCommand(0x03, moteurDroitPWM);
  RobotCom::addCommand(0x04, moteurGauchePWM);
  // Initialisation du compteur en quadrature
  pinMode(RSTX, OUTPUT);
  pinMode(RSTY, OUTPUT);
  pinMode(SEL1, OUTPUT);
  pinMode(SEL2, OUTPUT);
  pinMode(OE, OUTPUT);
  pinMode(XY,OUTPUT);
  

  digitalWrite(RSTX, LOW);    // Reset compteur X
  digitalWrite(RSTY, LOW);    // Reset compteur Y
  digitalWrite(RSTX, HIGH);   
  digitalWrite(RSTY, HIGH);
  digitalWrite(OE, HIGH);    

  // Initialisation du registre
  pinMode(LATCH, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(DATA, INPUT);

  digitalWrite(CLK, LOW);
  digitalWrite(LATCH, LOW);

  // Initialisation du driver moteur
  pinMode(RSTDRV,OUTPUT);
  pinMode(FAULTDRV,INPUT);
  pinMode(EN1,OUTPUT);
  pinMode(IN1_1,OUTPUT);
  pinMode(IN1_2,OUTPUT);
  pinMode(EN2,OUTPUT);
  pinMode(IN2_1,OUTPUT);
  pinMode(IN2_2,OUTPUT);
  
	digitalWrite(RSTDRV, LOW);
	digitalWrite(RSTDRV, HIGH);

 // Debug
  //Serial.begin(115200);
}

void loop()
{
  RobotCom::executeCommands();
  //updateCounter(0); 
  //updateCounter(1); 



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

void updateCounter(byte xy)
{
  digitalWrite(XY,xy);
  long& counter = (xy == LOW) ? rightCounter : leftCounter;
  counter = 0;
  digitalWrite(OE, LOW);
  // 		| MSB | 2ND | 3RD | LSB |
  // SEL1 |  0  |  1  |  0  |  1  |
  // SEL2 |  1  |  1  |  0  |  0  |
  for (int i = 0; i < 4; i++)
  {
    delayMicroseconds(100);
    byte sel1 = i & 0x01;
    byte sel2 = (~i >> 1) & 0x01;
    digitalWrite(SEL1, sel1);
    digitalWrite(SEL2, sel2);
    byte test = shiftInFixed(DATA, CLK);
    counter += test;
    // Debug
    /*Serial.print(i);
    Serial.print(":");
    Serial.print(test);
    Serial.print("\t");*/
    if (i != 3)
      counter <<= 8;
    

  }
  //Debug
  /*Serial.print("->");
  Serial.println(counter);*/
  digitalWrite(OE, HIGH);

}

byte shiftInFixed(byte dataPin, byte clockPin)
{
  digitalWrite(LATCH, HIGH);
  byte value = 0;
  for (int i = 0; i < 8; i++)
  {
    value <<= 1;
    value |= digitalRead(dataPin);
    digitalWrite(clockPin, HIGH);
    digitalWrite(clockPin, LOW);
  }
  digitalWrite(LATCH, LOW);
  return value;
}


