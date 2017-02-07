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
    SensorAv.attach(TRIGGPINAv, ECCHOPINAv);
    SensorAr.attach(TRIGGPINAr, ECCHOPINAr;
    SensorAv.trig();
    SensorAr.trig();
}


void loop() {
  talks.execute(); 
  SensorAv.update();
  SensorAr.update();
  if (SensorAv.getReady()) {
    SensorAv.trig();
  }
  if (SensorAr.getReady()){
      SensorAr.trig();
  }
}