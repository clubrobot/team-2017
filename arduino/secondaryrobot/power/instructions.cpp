#include "instructions.h"
#include "configuration.h"

void GET_EMERGENCY_STOP_STATE(SerialTalks &inst, Deserializer &input, Serializer &output)
{
    
}

void GET_VOLTAGE(SerialTalks &inst, Deserializer &input, Serializer &output)
{
    float voltage = analogRead(VOLTAGE_PIN);
	voltage = (voltage / VOLTAGE_MAX_NORMALIZED)*VOLTAGE_MAX;
	output << voltage;
}

void GET_BATTERY_CHARGE(SerialTalks &inst, Deserializer &input, Serializer &output)
{

}
