#include <Arduino.h>

#include "../../common/UltrasonicSensor.h"
#include "../../common/SerialTalks.h"
#include "instructions.h"
 

UltrasonicSensor Capteur1(ECHOPIN);

void setup() {  
  talks.begin(Serial);
  talks.bind(GET_MESURE_OPCODE, GET_MESURE);
  talks.bind(MESURE_OPCODE, MESURE);
}

void loop() {
    talks.execute(); 
    Capteur1.update();
    Capteur1.impulsion_US(TRIGGPIN);
    Capteur1.setcurrentState(EMISSION);  
}