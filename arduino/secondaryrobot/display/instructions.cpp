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
    input >> buffer;
    //output << buffer;

    int nbDigits = 0;
    int shift = 0;
    for (int i = 0; i < IP_DISPLAY_BUFFER_SIZE; i++)
    {
        if (buffer[i] >= '0' && buffer[i] <= '9')
        {
            nbDigits++;
        }
    }
	output << nbDigits;
    shift = (12 - nbDigits) / 2;
	output << shift;
    ipdisplay.computeBuffer(buffer, shift);
	output << ipdisplay.getData();
}
