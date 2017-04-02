#include <Arduino.h>
#include <Servo.h>
#include "PIN.h"
#include "instructions.h"
#include "../../common/DCMotor.h"
#include "../../common/SerialTalks.h"
#include "../../common/Clock.h"
#include "../../common/VelocityServo.h"
#include "../../common/EndStop.h"
#include "../../common/FullSpeedServo.h"

const float BRAKEVELOCITY = 2;


DCMotorsDriver motorDriver;
DCMotor gripperMotor;

VelocityServo gripper;
FullSpeedServo dispenser;

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
    talks.bind(_OPEN_GRIP_OPCODE, OPEN_GRIP);   
    talks.bind(_SET_GRIP_VELOCITY_OPCODE, SET_GRIP_VELOCITY);
    talks.bind(_GET_MOTOR_VELOCITY_OPCODE,GET_MOTOR_VELOCITY);


    pinMode(SERVO1, OUTPUT);
    dispenser.attach(SERVO1);
    dispenser.enable();
    dispenser.setTimestep(0.1);
    
    highStop.attach(SWITCH4);
    lowStop.attach(SWITCH3);

    pinMode(SERVO2, OUTPUT);
    gripper.attach(SERVO2);
    gripper.enable();
    gripper.setTimestep(0.01);

    motorDriver.attach(DRIVER_RESET , A7);
    motorDriver.reset();
    
    gripperMotor.attach(MOTOR2_EN, MOTOR2_PWM, MOTOR2_DIR);
    gripperMotor.setConstant(1/11.1);

    // Miscellanous
	TCCR2B = (TCCR2B & 0b11111000) | 1; // Set Timer2 frequency to 16MHz instead of 250kHz
    gripper.detach();   //Empeche les deux servos de forcer pour garder leur position au démarrage de l'arduino
    dispenser.detach();
}

void loop(){
     talks.execute();
     gripper.update();
     dispenser.update();

     if(highStop.getState() && gripperMotor.getVelocity()<BRAKEVELOCITY*(-1)){
        gripperMotor.setVelocity(BRAKEVELOCITY*(-1));
     }
     else if(lowStop.getState() && gripperMotor.getVelocity()>BRAKEVELOCITY){
        gripperMotor.setVelocity(BRAKEVELOCITY);
     }
}




