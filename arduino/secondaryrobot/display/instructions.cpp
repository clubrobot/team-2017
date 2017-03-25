#include <Arduino.h>
#include "instructions.h"
#include "IPDisplay.h"
#include "ledMatrix.h"
#include <EEPROM.h>
#include "eepromManagment.h"

// Global variables
extern Ipdisplay ipdisplay;
extern LedMatrix ledmatrix1;
extern LedMatrix ledmatrix2;
extern LedMatrix ledmatrix3;


// Instructions
void SET_MATRIX_MESSAGE(SerialTalks &talks, Deserializer &input, Serializer &output)
{
	// parametres a recuperer : 
	// numero de la matrice
	// type d'affichage (continu, défilement)
	// chaine de caractère a afficher
	byte matrixID = input.read<byte>();			// ID of the matrix to be updates. Set to 0 to send the data to all the matrix
	byte mode = input.read<byte>();				// Mode of the matrix {SLIDE_MODE, ANIMATION_MODE}
	char buffer[NB_PATTERNS_MAX] = "";
	for (int i = 0; i< NB_PATTERNS_MAX; i++){
		buffer[i] = input.read<char>();
	}
	ledmatrix1.setMode(mode);
	ledmatrix1.computeBuffer(buffer);
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


void SET_EEPROM_CHAR_IPDISPLAY(SerialTalks &talks, Deserializer &input, Serializer &output)
{
	char character = input.read<char>();		// The char to change
	char data = input.read<char>();				// The segments to display
	EEPROM.write(EEPROM_IPDISPLAY_START_ADDRESS + character - START_CHAR,data);
}


//void SET_EEPROM_CHAR_LEDMATRIX(SerialTalks &talks, Deserializer &input, Serializer &output);

void SET_EEPROM_SPEED_MATRIX(SerialTalks &talks, Deserializer &input, Serializer &output)
{
	byte idMatrix = input.read<byte>();			// The matrix ID
	int speed = input.read<int>();				// The pattern speed in ms
	switch(idMatrix){
		case 1 :
			EEPROMWriteInt(EEPROM_LEDMATRIX1_TIMESTEP_START_ADDRESS,speed);
			break;
		case 2 :
			EEPROMWriteInt(EEPROM_LEDMATRIX2_TIMESTEP_START_ADDRESS,speed);
			break;
		case 3 :
			EEPROMWriteInt(EEPROM_LEDMATRIX3_TIMESTEP_START_ADDRESS,speed);
	}
	
}

