#include "instructions.h"

#include "../../common/SerialTalks.h"
#include "../../common/DCMotor.h"
#include <Servo.h>


// Global variables
extern Servo dispenser;
extern DCMotorsDriver driver;
extern DCMotor motor1;
extern DCMotor motor2;


// Instructions
void SET_DISPENSER_SETPOINT(SerialTalks& talks, Deserializer& input, Serializer& output)
{
	dispenser.write(input.read<int>());
}

void SET_MOTOR1_VELOCITY(SerialTalks& talks, Deserializer& input, Serializer& output)
{
	float motorVelocity  = input.read<float>();
	motor1.setVelocity(motorVelocity);
}

void SET_MOTOR2_VELOCITY(SerialTalks& talks, Deserializer& input, Serializer& output)
{
	float motorVelocity  = input.read<float>();
	motor2.setVelocity(motorVelocity);
}




