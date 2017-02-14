#include "instructions.h"
#include "EndStop.h"

extern Servo gripper;
extern Servo Dispenser;
extern EndStop highStop;
extern EndStop lowStop;

void WRITE_GRIP(SerialTalks& inst, Deserializer& input, Serializer& ouput){
    gripper.write(input.read<int>());
}

void WRITE_DISPENSER(SerialTalks& inst, Deserializer& input, Serializer& ouput){
    dispenser.write(input.read<int>());
}

bool IS_UP(SerialTalks& inst, Deserializer& input, Serializer& ouput){
    output.write<int>(highEndStop.getState());
}

bool IS_DOWN(SerialTalks& inst, Deserializer& input, Serializer& ouput){
    output.write<int>(lowEndStop.getState());
}

void setMotorVelocity(SerialTalks& inst, Deserializer& input, Serializer& ouput){

}