#include <Arduino.h>
#include "instructions.h"
#include "IPDisplay.h"

// Global variables
extern Ipdisplay ipdisplay;

// Instructions
void SET_MATRIX_MESSAGE(SerialTalks &talks, Deserializer &input, Serializer &output)
{
}

void SET_IPDISPLAY_MESSAGE(SerialTalks &talks, Deserializer &input, Serializer &output)
{

    byte value[MAXVAL];
    for (int i = 0; i < MAXVAL; i++)
    {
        value[i] = -1;
    }

    if (Serial.available() > 0)
    {
        int cpt = 0;
        Serial.println();
        //Serial.print("nouveau message : ");
        while (Serial.available() > 0)
        {
            value[cpt] = Serial.read();
            //Serial.println(value[cpt]);
            value[cpt] -= '0';
            cpt++;
        }
        cpt = 0;
        int nbDigits = 0;
        int shift = 0;
        for (int i = 0; i < MAXVAL; i++)
        {
            if (value[cpt] >= 0 && value[cpt] < ('9' - '0'))
            {
                nbDigits++;
            }
            cpt++;
        }
        shift = (12 - nbDigits) / 2;
        ipdisplay.computeValues(value, shift);
    }
}
