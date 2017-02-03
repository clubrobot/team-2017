#include <Arduino.h>
#include <Servo.h>
#include "PIN.h"
#include "instructions.h"

#include "../../common/SerialTalks.h"


// instructions.cpp
extern Servo dispenser;

void SET_DISPENSER_SETPOINT(SerialTalks& talks, Deserializer& input, Serializer& output)
{
	dispenser.write(input.read<int>());
}

// modulescollector.ino
Servo dispenser;

void setup()
{
	Serial.begin(SERIALTALKS_BAUDRATE);
	talks.begin(Serial);
	talks.bind(SET_DISPENSER_SETPOINT_OPCODE, SET_DISPENSER_SETPOINT);

	pinMode(DISPENSER_SERVO_PIN, OUTPUT);
	dispenser.attach(DISPENSER_SERVO_PIN);
}

void loop()
{
	talks.execute();
}
