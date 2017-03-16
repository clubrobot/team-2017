#include "instructions.h"
#include "../../common/EndStop.h"
#include "../../common/DCMotor.h"
#include <Servo.h>
#include "PIN.h"

extern Servo gripper;
extern Servo dispenser;
extern EndStop highStop;
extern EndStop lowStop;
extern DCMotor gripperMotor;

void WRITE_GRIP(SerialTalks& inst, Deserializer& input, Serializer& output){
    int val = input.read<int>();
    if(val>=0){
        if(!gripper.attached()){
            gripper.attach(SERVO2);
        }
        gripper.write(val);
    }
    else if(val <0){
        gripper.detach(); 
    }
}

void WRITE_DISPENSER(SerialTalks& inst, Deserializer& input, Serializer& output){
    int val = input.read<int>();
    if(val>=0){
        if(!dispenser.attached()){
            dispenser.attach(SERVO1);
        }
        dispenser.write(val);
    }
    else if(val <0){
        dispenser.detach(); 
    }
}

void IS_UP(SerialTalks& inst, Deserializer& input, Serializer& output){
    output.write<int>(highStop.getState());
}

void IS_DOWN(SerialTalks& inst, Deserializer& input, Serializer& output){
    output.write<int>(lowStop.getState());
}

void SET_MOTOR_VELOCITY(SerialTalks& inst, Deserializer& input, Serializer& output){
    float vel = input.read<float>();
    if(gripper.attached() && gripper.read()>5){
            gripperMotor.setVelocity(0);
            gripper.write(5);
            delay(500);
        }
    if(!gripper.attached()){
        gripperMotor.setVelocity(0);
        gripper.attach(SERVO2);
        gripper.write(5);
        delay(500);
        }
    gripperMotor.setVelocity(vel);
}