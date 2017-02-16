#include <Arduino.h>
#include <Servo.h>
#include "../../common/SerialTalks.h"
#include "../../common/DCMotor.h"
#include "PIN.h"
#include "instructions.h"
#include "configuration.h"


// modulescollector.ino
Servo dispenser;
DCMotorsDriver driver;
DCMotor motor1;

void setup()
{
	// Serial 
	Serial.begin(SERIALTALKS_BAUDRATE);
	talks.begin(Serial);
	// Functions available with opcode
	talks.bind(SET_DISPENSER_SETPOINT_OPCODE, SET_DISPENSER_SETPOINT);
	talks.bind(SET_MOTOR1_VELOCITY_OPCODE, SET_MOTOR1_VELOCITY);
	talks.bind(SET_MOTOR2_VELOCITY_OPCODE, SET_MOTOR2_VELOCITY);


	// Actuators init

	// Servos
	pinMode(DISPENSER_SERVO_PIN, OUTPUT);
	dispenser.attach(DISPENSER_SERVO_PIN);
	dispenser.write(74); // stop the servo

	// Switchs

	// Motors driver
	driver.attach(DRIVER_RESET,A7);
	driver.reset();

	// DC Motors
	motor1.attach(MOTOR1_EN, MOTOR1_PWM, MOTOR1_DIR);
	motor1.setRadius(RADIUS_MOTOR1);
	motor1.setConstants(VELOCITY_CONSTANT_MOTOR1, REDUCTION_RATIO_MOTOR1);
	motor1.setSuppliedVoltage(MOTORS_SUPPLY_VOLTAGE);

	motor2.attach(MOTOR2_EN, MOTOR2_PWM, MOTOR2_DIR);
	motor2.setRadius(RADIUX_MOTOR2);
	motor2.setConstants(VELOCITY_CONSTANT_MOTOR2, REDUCTION_RATIO_MOTOR2);
	motor2.setSuppliedVoltage(MOTORS_SUPPLY_VOLTAGE);


}

void loop()
{
	talks.execute();
	
}
