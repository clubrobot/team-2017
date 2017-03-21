#include <Arduino.h>
#include "instructions.h"
#include "IPDisplay.h"
#include "ledMatrix.h"
#include <EEPROM.h>

// Global variables
extern Ipdisplay ipdisplay;
extern LedMatrix ledmatrix;

// Instructions
void SET_MATRIX_MESSAGE(SerialTalks &talks, Deserializer &input, Serializer &output)
{
	// parametres a recuperer : 
	// numero de la matrice
	// type d'affichage (continu, défilement)
	// chaine de caractère a afficher
	int matrixID = input.read<int>();			// ID of the matrix to be updates. Set to 0 to send the data to all the matrix
	int shift = input.read<int>();				// Number of columns to shift every PATTERN_TIMESTEP
	char buffer[NB_PATTERNS_MAX] = "";
	for (int i = 0; i< NB_PATTERNS_MAX; i++){
		buffer[i] = input.read<char>();
	}
	ledmatrix.setShift(shift);
	ledmatrix.computeBuffer(buffer);
}

void SET_IPDISPLAY_MESSAGE(SerialTalks &talks, Deserializer &input, Serializer &output)
{
	
    char buffer[IP_DISPLAY_BUFFER_SIZE] = "";
	for (int i = 0; i< IP_DISPLAY_BUFFER_SIZE; i++){
		buffer[i] = input.read<char>();
	}

	int nbDigits = 0;
	int shift = 0;
	for (int i = 0; i < IP_DISPLAY_BUFFER_SIZE && buffer[i]!='\0'; i++)
    {
        if (buffer[i] >= START_CHAR && buffer[i] <= END_CHAR)
        {
            nbDigits++;
			if(buffer[i+1] == '.' && buffer[i] != '.'){	// gère le cas de l'affichage d'un point après un digit
				i++;
			}
			if(nbDigits>12){		// Buffer overflow case
				nbDigits--;
				buffer[i] = 0;
			}
        }
    }
	shift = (12 - nbDigits) / 2;
	ipdisplay.computeBuffer(buffer, shift);
}


#ifdef INIT_EEPROM
	void INIT_EEPROM_DATA(SerialTalks &talks, Deserializer &input, Serializer &output)
	{
		for(int i = 0;i<EEPROM_IPDISPLAY_LENGTH;i++){
			EEPROM.write(EEPROM_IPDISPLAY_START_ADDRESS+i,segToDisplay[i]);	
		}
	}
#endif