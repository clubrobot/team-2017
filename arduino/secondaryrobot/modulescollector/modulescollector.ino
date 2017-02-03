#include <Arduino.h>
#include <Servo.h>

#include "../../common/SerialTalks.h"

// PIN.h
#define DISPENSER_SERVO 2

// instructions.h
#define DISPENSER_WRITE_OPCODE 0x0D

// instructions.cpp
extern Servo dispenser;

void DISPENSER_WRITE(SerialTalks& talks, Deserializer& input, Serializer& output)
{
	dispenser.write(input.read<int>());
}

// modulescollector.ino
Servo dispenser;

void setup()
{
	Serial.begin(SERIALTALKS_BAUDRATE);
	talks.begin(Serial);
	talks.bind(DISPENSER_WRITE_OPCODE, DISPENSER_WRITE);

	pinMode(DISPENSER_SERVO, OUTPUT);
	dispenser.attach(DISPENSER_SERVO);
	dispenser.write(74);
}

void loop()
{
	talks.execute();
}
