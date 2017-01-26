#include "instructions.h"
#include "../../common/UltrasonicSensor.h"


// Global variables
extern UltrasonicSensor Capteur1;
extern UltrasonicSensor Capteur2;

void GET_MESURE(SerialTalks& inst, Deserializer& input, Serializer& output)
{
    output.write<int>(Capteur1.getDistance());*
    output.write<int>(Capteur2.getDistance());
}

void MESURE(SerialTalks& inst, Deserializer& input, Serializer& output)
{
    Capteur1.update();
    if(Capteur1.getState() == PRET){
        Capteur1.impulsion_US(TRIGGPIN);
        Capteur1.setcurrentState(EMISSION);
    }
}

