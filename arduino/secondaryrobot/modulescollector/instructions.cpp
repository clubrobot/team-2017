#include "instructions.h"
#include "../../common/EndStop.h"
#include "../../common/DCMotor.h"
#include "../../common/VelocityServo.h"
#include <Servo.h>
#include "PIN.h"

#define GRIP_CYLINDER_ANGLE 40
#define GRIP_VELOCITY 360

extern VelocityServo gripper;
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
    int val = input.read<int>();
    if (val >= 0)
    {
        if (!gripper.attached())
        {
            gripper.attach(SERVO1);
        }
        gripper.write(GRIP_CYLINDER_ANGLE);
        gripper.Velocitywrite(GRIP_VELOCITY, val);
    }
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
        }
    }
    gripperMotor.setVelocity(vel);
}