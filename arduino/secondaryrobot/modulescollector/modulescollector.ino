#include <Arduino.h>
#include <Servo.h>
#include "../../common/EndStop.h"
#include "PIN.h"
#include "instructions.h"
#include "../../common/DCMotor.h"
#include "../../common/SerialTalks.h"
#include "../../common/Clock.h"

#define MAXMOVINGTIME 5000000

const float BRAKEVELOCITY = 0.16;


DCMotorsDriver motorDriver;
DCMotor gripperMotor;

Servo gripper;
Servo dispenser;

EndStop highStop;
EndStop lowStop;

Clock MovingTime;
bool MotorIsMoving = false;

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
    dispenser.attach(SERVO1);
    gripper.attach(SERVO2);
    highStop.attach(SWITCH4);
    lowStop.attach(SWITCH3);

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
     if(!MotorIsMoving && (gripperMotor.getVelocity() != 0 || abs(gripperMotor.getVelocity()) != BRAKEVELOCITY)){
         MovingTime.restart();
         MotorIsMoving = true;    
     }
     
     else if(MotorIsMoving){
        if(gripperMotor.getVelocity() == 0 || abs(gripperMotor.getVelocity()) == BRAKEVELOCITY){ 
            MotorIsMoving = false;
        }
        else if(gripper.read()>5){
            float vel = gripperMotor.getVelocity();
            gripperMotor.setVelocity(0);
            gripper.write(5);
            delay(20);
            gripperMotor.setVelocity(vel);
        }
         else if(MovingTime.getElapsedTime() > MAXMOVINGTIME){
            gripperMotor.setVelocity(0);
        }
     }

     if(highStop.getState() && gripperMotor.getVelocity()<BRAKEVELOCITY*(-1)){
        gripperMotor.setVelocity(BRAKEVELOCITY*(-1));
     }
     else if(lowStop.getState() && gripperMotor.getVelocity()>BRAKEVELOCITY){
        gripperMotor.setVelocity(BRAKEVELOCITY);
     }
}




