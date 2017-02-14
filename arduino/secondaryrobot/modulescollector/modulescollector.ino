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
    
    pinMode(SERVODISPENSER, OUTPUT);
    pinMode(SERVOGRIP, OUTPUT);
    gripper.attach(SERVOGRIP);
    dispenser.attach(SERVODISPENSER);
    highStop.attach(HIGHENDSTOP);
    lowStop.attach(LOWENDSTOP);

//    motorDriver.attach(reset , gdk);
//    motorDriver.reset();
    
//    gripperMotor.attach( EN, PWM, DIR);
    gripperMotor.setConstants(300/6, 1);
    gripperMotor.setSuppliedVoltage(11);
    gripperMotor.setRadius(8);
}

void loop(){
     talks.execute();
}




