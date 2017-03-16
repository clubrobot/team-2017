#include "instructions.h"
#include "../../common/EndStop.h"
#include "../../common/DCMotor.h"
#include <Servo.h>
#include "PIN.h"

#define GRIP_CYLINDER_ANGLE 45
#define HIGH_OPEN_ANGLE 45

extern Servo gripper;
extern Servo dispenser;
extern EndStop highStop;
extern EndStop lowStop;
extern DCMotor gripperMotor;

void WRITE_GRIP(SerialTalks &inst, Deserializer &input, Serializer &output)
{
    int val = input.read<int>();
    if (val >= 0)
    {
        if (!gripper.attached())
        {
            gripper.attach(SERVO2);
        }
        gripper.write(val);
    }
    else if (val < 0)
    {
        gripper.detach();
    }
}

void OPEN_SLOWLY(SerialTalks &inst, Deserializer &input, Serializer &output)
{
    float o = GRIP_CYLINDER_ANGLE;
    int res = 200;
    float t = input.read<float>();
    while (o <= HIGH_OPEN_ANGLE){
        gripper.write(o);
        o+=(HIGH_OPEN_ANGLE - GRIP_CYLINDER_ANGLE)/res;
        delay(t * 1000/res);
    }
    gripper.write(HIGH_OPEN_ANGLE);
}

void WRITE_DISPENSER(SerialTalks &inst, Deserializer &input, Serializer &output)
{
    int val = input.read<int>();
    if (val >= 0)
    {
        if (!dispenser.attached())
        {
            dispenser.attach(SERVO1);
        }
        dispenser.write(val);
    }
    else if (val < 0)
    {
        dispenser.detach();
    }
}

void IS_UP(SerialTalks &inst, Deserializer &input, Serializer &output)
{
    output.write<int>(highStop.getState());
}

void IS_DOWN(SerialTalks &inst, Deserializer &input, Serializer &output)
{
    output.write<int>(lowStop.getState());
}

void SET_MOTOR_VELOCITY(SerialTalks &inst, Deserializer &input, Serializer &output)
{
    float vel = input.read<float>();
    if (highStop.getState() || lowStop.getState())
    {
        if (gripper.attached() && gripper.read() > 5)
        {
            gripperMotor.setVelocity(0);
            gripper.write(5);
            delay(200);
        }
        if (!gripper.attached())
        {
            gripperMotor.setVelocity(0);
            gripper.attach(SERVO2);
            gripper.write(5);
            delay(200);
        }
    }
    gripperMotor.setVelocity(vel);
}