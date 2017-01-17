#include <Arduino.h>

#include "../../common/UltrasonicSensor.h"
#include "../../common/SerialTalks.h"
#include "instruction.h"
 
int triggPin = 7;
int echoPin1 = 5;  

UltrasonicSensor Capteur1(echoPin1);

void setup() {  
  talks.begin(Serial);
  talks.bind(GET_MESURE_OPCODE, GET_MESURE);
  talks.bind(MESURE_OPCODE, MESURE);
}

void loop() {
    talks.execute();    
}