#include <arduino.h>
#include <servo.h>
#include "EndStop.h"
#include "PIN.h"
#include "instructions.h"
#include "DCMotor.h"


DCMotorDriver MotorDriver
DCMotor GripperMotor;

Servo gripper;
Servo dispenser;

EndStop highStop;
EndStop lowStop;


void setup(){
    Serial.begin(SERIALTALKS_BAUDRATE);
    talks.begin(Serial);
    talKs.bind(_WRITE_DISPENSER_OPCODE, WRITE_DISPENSER);
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

    Driver.attach(reset , gdk);
    Driver.reset();
    
    GripperMotor.attach( EN, PWM, DIR);
    GripperMotor.setConstants(300/6, 1);
    GripperMotor.setSuppliedVoltage(11);
    GripperMotor.setRadius(8);
}

void loop(){
     talks.execute();
     GripperMotor.update();
}




