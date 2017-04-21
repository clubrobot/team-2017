#include "instructions.h"
#include "../common/UltrasonicSensor.h"


// Global variables
extern UltrasonicSensor SensorAr;
extern UltrasonicSensor SensorAv;

void GET_MESURE(SerialTalks& inst, Deserializer& input, Serializer& output)
{
    output.write<int>(SensorAr.getMesure());
    output.write<int>(SensorAv.getMesure());
}


