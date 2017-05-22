#include "instructions.h"
#include "../common/UltrasonicSensor.h"


// Global variables
extern UltrasonicSensor SensorAr;
extern UltrasonicSensor SensorAv;
extern bool activated; 

void GET_MESURE(SerialTalks& inst, Deserializer& input, Serializer& output)
{
    output.write<int>(SensorAr.getMesure());
    output.write<int>(SensorAv.getMesure());
}

void ACTIVATE_SENSORS(SerialTalks& inst, Deserializer& input, Serializer& output){
    activated = input.read<bool>();
}

void DESACTIVATE_SENSORS(SerialTalks& inst, Deserializer& input, Serializer& output){
    activated = input.read<bool>();
}
