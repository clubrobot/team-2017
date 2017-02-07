#include <Arduino.h>

#include "../../common/UltrasonicSensor.h"
#include "../../common/SerialTalks.h"
#include "../../common/EchoHandler.h"
#include "instructions.h"
#include "PIN.h"
 

UltrasonicSensor SensorAv;
UltrasonicSensor SensorAr;

void setup() {
    Serial.begin(SERIALTALKS_BAUDRATE);
    talks.begin(Serial);
    talks.bind(GET_MESURE_OPCODE, GET_MESURE);
    talks.bind(MESURE_OPCODE, MESURE);
    SensorAv.attach(7, 2);
    SensorAr.attach(8, 3);
    SensorAv.trig();
    SensorAr.trig();
}

void loop() {
    talks.execute(); 
    Capteur1.update();
    Capteur2.update();
    if(Capteur1.getState() == PRET && Capteur2.getState() == PRET){
        Capteur1.impulsion_US(TRIGGPIN);
        Capteur2.impulsion_US(TRIGGPIN);
        Capteur1.setcurrentState(EMISSION);
        Capteur2.setcurrentState(EMISSION);
    }
}

void loop() {
  // put your main code here, to run repeatedly:
  SensorAv.update();
  SensorAr.update();
  if (SensorAv.getReady()) {
    SensorAv.trig();
  }
  if (SensorAr.getReady()){
      SensorAr.trig();
  }
}