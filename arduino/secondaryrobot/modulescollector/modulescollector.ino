#include <Arduino.h>
#include "../../common/EndStop.h"
#include "PIN.h"
#include "instructions.h"
#include <Servo.h>
#include "../../common/DCMotor.h"
#include "../../common/SerialTalks.h"
#include "../../common/Clock.h"
#include "../../common/VelocityServo.h"

#define MAXMOVINGTIME 4000000

const float BRAKEVELOCITY = 0.16;


DCMotorsDriver motorDriver;
DCMotor gripperMotor;

VelocityServo gripper;
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
    dispenser.attach(SERVO1);
    
    highStop.attach(SWITCH4);
    lowStop.attach(SWITCH3);

    pinMode(SERVO2, OUTPUT);
    gripper.attach(SERVO2);
    gripper.enable();
    gripper.setTimestep(0.05);

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
     
     if(!MotorIsMoving && (gripperMotor.getVelocity() != 0 || abs(gripperMotor.getVelocity()) != BRAKEVELOCITY)){
         MovingTime.restart();
         MotorIsMoving = true;    
     }
     
     else if(MotorIsMoving){
        if(gripperMotor.getVelocity() == 0 || abs(gripperMotor.getVelocity()) == BRAKEVELOCITY){ 
            MotorIsMoving = false;
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




