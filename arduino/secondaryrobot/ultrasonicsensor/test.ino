#include <Arduino.h>

#include "../../common/UltrasonicSensor.h"
#include "../../common/SerialTalks.h"
#include "instructions.h"
#include "PIN.h"
 

UltrasonicSensor Capteur1(ECHOPIN);

void setup() {
  Serial.begin(SERIALTALKS_BAUDRATE);
  talks.begin(Serial);
  talks.bind(GET_MESURE_OPCODE, GET_MESURE);
  talks.bind(MESURE_OPCODE, MESURE);
}

void loop() {
    talks.execute(); 
    Capteur1.update();
    if(Capteur1.getState() == PRET){
        Capteur1.impulsion_US(TRIGGPIN);
        Capteur1.setcurrentState(EMISSION);
    }
}