#include "instructions.h"
#include "../../common/UltrasonicSensor.h"

// Global variables
extern UltrasonicSensor Capteur1;

 bool GET_MESURE(SerialTalks& inst, Deserializer& input, Serializer& output)
{
    output.write<int>(sensor.getDistance());
	return false;
} 

bool MESURE(SerialTalks& inst, Deserializer& input, Serializer& output)
{
    Capteur1.update();
    Capteur1.impulsion_US(triggPin);
    Capteur1.setcurrentState(EMISSION);
    return false;
}

