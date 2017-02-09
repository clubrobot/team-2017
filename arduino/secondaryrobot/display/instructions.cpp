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

    char buffer[IP_DISPLAY_BUFFER_SIZE];
    for (int i = 0; i < IP_DISPLAY_BUFFER_SIZE; i++)
    {
        buffer[i] = -1;
    }
    input >> buffer;

    int cpt = 0;
    int nbDigits = 0;
    int shift = 0;
    for (int i = 0; i < IP_DISPLAY_BUFFER_SIZE; i++)
    {
        if (buffer[cpt] >= '0' && buffer[cpt] <= '9')
        {
            nbDigits++;
        }
        cpt++;
    }
    shift = (12 - nbDigits) / 2;
    ipdisplay.computeBuffer(buffer, shift);
}
