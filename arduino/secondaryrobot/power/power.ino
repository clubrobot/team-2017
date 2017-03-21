#include <Arduino.h>
#include "PIN.h"
#include "instructions.h"
#include "../../common/SerialTalks.h"



void setup(){
    Serial.begin(SERIALTALKS_BAUDRATE);
    talks.begin(Serial);

    talks.bind(_GET_EMERGENCY_STOP_STATE_OPCODE, GET_EMERGENCY_STOP_STATE);
    talks.bind(_GET_VOLTAGE_OPCODE, GET_VOLTAGE);
    talks.bind(_GET_BATTERY_CHARGE_OPCODE, GET_BATTERY_CHARGE);  
    
}

void loop(){
     talks.execute();   
}




