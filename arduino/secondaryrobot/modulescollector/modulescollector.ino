#include <Arduino.h>
#include <Servo.h>
#include "../../common/EndStop.h"
#include "PIN.h"
#include "instructions.h"
#include "../../common/DCMotor.h"
#include "../../common/SerialTalks.h"


DCMotorsDriver motorDriver;
DCMotor gripperMotor;

Servo gripper;
Servo dispenser;

EndStop highStop;
EndStop lowStop;


void setup(){
    Serial.begin(SERIALTALKS_BAUDRATE);
    talks.begin(Serial);
    talks.bind(_WRITE_DISPENSER_OPCODE, WRITE_DISPENSER);
    talks.bind(_WRITE_GRIP_OPCODE, WRITE_GRIP);
    talks.bind(_IS_UP_OPCODE, IS_UP);
    talks.bind(_IS_DOWN_OPCODE, IS_DOWN);
    talks.bind(_SET_MOTOR_VELOCITY_OPCODE, SET_MOTOR_VELOCITY);    
    
    pinMode(SERVO1, OUTPUT);
    pinMode(SERVO2, OUTPUT);
    gripper.attach(SERVO1);
    dispenser.attach(SERVO2);
    highStop.attach(SWITCH1);
    lowStop.attach(SWITCH2);

    motorDriver.attach(DRIVER_RESET , A7);
    motorDriver.reset();
    
    gripperMotor.attach(MOTOR1_EN, MOTOR1_PWM, MOTOR1_DIR);
    gripperMotor.setConstants(300/6, 1);
    gripperMotor.setSuppliedVoltage(11);
    gripperMotor.setRadius(8);
}

void loop(){
     talks.execute();
}




