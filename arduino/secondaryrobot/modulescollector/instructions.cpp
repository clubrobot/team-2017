#include "instructions.h"
#include "../../common/EndStop.h"
#include "../../common/DCMotor.h"
#include <Servo.h>

extern Servo gripper;
extern Servo dispenser;
extern EndStop highStop;
extern EndStop lowStop;
extern DCMotor gripperMotor;

void WRITE_GRIP(SerialTalks& inst, Deserializer& input, Serializer& output){
    gripper.write(input.read<int>());
}

void WRITE_DISPENSER(SerialTalks& inst, Deserializer& input, Serializer& output){
    dispenser.write(input.read<int>());
}

void IS_UP(SerialTalks& inst, Deserializer& input, Serializer& output){
    output.write<int>(highStop.getState());
}

void IS_DOWN(SerialTalks& inst, Deserializer& input, Serializer& output){
    output.write<int>(lowStop.getState());
}

void SET_MOTOR_VELOCITY(SerialTalks& inst, Deserializer& input, Serializer& output){
    float vel = input.read<float>();
    if(vel <= 400){
        gripperMotor.setVelocity(vel);
        talks.out << vel << "\n";
    }
}