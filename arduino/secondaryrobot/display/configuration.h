#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "../../common/SerialTalks.h"

//************************
// initialisation EEPROM *
//************************



//************
// Constants *
//************


// Time constants
#define IP_DISPLAY_TIMESTEP 3e-3    // Execution step time in s
#define LED_MATRIX_TIMESTEP 2e-4    // Execution step time in s
#define PATTERN_TIMESTEP 100e-3    // Execution step time in s

// IP_display constants
#define IP_DISPLAY_BUFFER_SIZE 25   // Size of the data buffer receiver
#define DISP_NUMBER 3               // Number of 7 seg common pin

// Led matrix constants
//TODO




//*****************************
// 7seg Display configuration *
//*****************************

// common pin DISP1 , DISP2 , DISP3
const byte DISP_PIN[] = {4, 3, 2};

// shift register pins
#define DATA_IPDISPLAY 5
#define CLOCK_IPDISPLAY 6
#define LATCH_IPDISPLAY 7

//ASCII TABLE => from 32 to 126
//a,b,c,d,e,f,g,dp  
#define START_CHAR ' '
#define END_CHAR '~'

//Weight of each segment
//                         a , b , c , d , e , f , g , dp
const byte DISP[4][8] = {{1, 128, 16, 4, 2, 64, 32, 8},
                         {1, 128, 32, 8, 4, 2, 64, 16},
                         {1, 128, 16, 4, 2, 64, 32, 8},		//{128, 1, 8, 32, 64, 2, 4, 16},
                         {1, 128, 32, 8, 4, 2, 64, 16}};

//***************************
// LED Matrix Configuration *
//***************************

// shift register pins (Matrix 1)
#define DATA_MATRIX1 8
#define CLOCK_MATRIX1 9
#define LATCH_MATRIX1 10

// shift register pins (Matrix 2)
#define DATA_MATRIX2
#define CLOCK_MATRIX2
#define LATCH_MATRIX2

// shift register pins (Matrix 3)
#define DATA_MATRIX3
#define CLOCK_MATRIX3
#define LATCH_MATRIX3

// general configuration

#define ROTATION_MATRIX_1 90
#define NB_PATTERNS_MAX 20
enum {SLIDE_MODE,ANIMATION_MODE};

// Weight of each dot
const unsigned int rows[] = {4, 16384, 8, 32768, 128, 1, 512, 16}; //Row1,Row2,...,Row8
const unsigned int cols[] = {256, 2048, 64, 2, 32, 4096, 8192, 1024}; //Col1,Col2,...,Col8

// Alphabet

//TODO : compléter table Ascii

#define HEART1 { \
    0b01101100,  \
    0b10111110, \
    0b10111110, \
    0b10111110, \
    0b01011100, \
    0b00111000, \
    0b00010000, \
    0b00000000, \
  }

#define HEART2 { \
    0b00000000,  \
    0b00101000, \
    0b01011100, \
    0b01011100, \
    0b00101000, \
    0b00010000, \
    0b00000000, \
    0b00000000, \
  }

#define HEART3 { \
    0b00000000,  \
    0b00000000, \
    0b00101000, \
    0b00111000, \
    0b00010000, \
    0b00000000, \
    0b00000000, \
    0b00000000, \
  }

#define SPACE { \
    0b00000000,  \
    0b00000000, \
    0b00000000, \
    0b00000000, \
    0b00000000, \
    0b00000000, \
    0b00000000, \
    0b00000000, \
  }

#define APOSTROPHE { \
    0b00001000,  \
    0b00010000, \
    0b00100000, \
    0b00000000, \
    0b00000000, \
    0b00000000, \
    0b00000000, \
    0b00000000, \
  }


#define LETTER_A { \
    0b00011000,  \
    0b00100100, \
    0b01000010, \
    0b01000010, \
    0b01111110, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
  }

#define LETTER_B { \
    0b01111100,  \
    0b01000010, \
    0b01000010, \
    0b01111100, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b01111100, \
  }

#define LETTER_C { \
    0b00011110,  \
    0b00100000, \
    0b01000000, \
    0b01000000, \
    0b01000000, \
    0b01000000, \
    0b00100000, \
    0b00011110, \
  }

#define LETTER_D { \
    0b01111000,  \
    0b01000100, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b01000100, \
    0b01111000, \
  }

#define LETTER_E  { \
    0b01111110,  \
    0b01000000, \
    0b01000000, \
    0b01111110, \
    0b01000000, \
    0b01000000, \
    0b01000000, \
    0b01111110, \
  }

#define LETTER_F { \
    0b01111110,  \
    0b01000000, \
    0b01000000, \
    0b01111100, \
    0b01000000, \
    0b01000000, \
    0b01000000, \
    0b01000000, \
  }

#define LETTER_G { \
    0b00011110,  \
    0b00100000, \
    0b01000000, \
    0b01000000, \
    0b01001110, \
    0b01000010, \
    0b00100010, \
    0b00011100, \
  }

#define LETTER_H { \
    0b01000010,  \
    0b01000010, \
    0b01000010, \
    0b01111110, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
  }

#define LETTER_I { \
    0b00111110,  \
    0b00001000, \
    0b00001000, \
    0b00001000, \
    0b00001000, \
    0b00001000, \
    0b00001000, \
    0b00111110,  \
  }

#define LETTER_J { \
    0b01111110,  \
    0b00001000, \
    0b00001000, \
    0b00001000, \
    0b00001000, \
    0b00001000, \
    0b00001000, \
    0b00110000, \
  }

#define LETTER_K { \
    0b01000100,  \
    0b01001000, \
    0b01010000, \
    0b01100000, \
    0b01100000, \
    0b01010000, \
    0b01001000, \
    0b01000100, \
  }

#define LETTER_L { \
    0b01000000,  \
    0b01000000, \
    0b01000000, \
    0b01000000, \
    0b01000000, \
    0b01000000, \
    0b01000000, \
    0b01111110, \
  }

#define LETTER_M { \
    0b01000010,  \
    0b01100110, \
    0b01011010, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
  }

#define LETTER_N { \
    0b01000010,  \
    0b01100010, \
    0b01010010, \
    0b01001010, \
    0b01000110, \
    0b01000010, \
    0b01000000, \
    0b01000000, \
  }

#define LETTER_O { \
    0b00011000,  \
    0b00100100, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b00100100, \
    0b00011000, \
  }

#define LETTER_P { \
    0b01111100,  \
    0b01000010, \
    0b01000010, \
    0b01111100, \
    0b01000000, \
    0b01000000, \
    0b01000000, \
    0b01000000, \
  }

#define LETTER_Q { \
    0b00011000,  \
    0b00100100, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b01001010, \
    0b00100100, \
    0b00011010, \
  }

#define LETTER_R { \
    0b01111100,  \
    0b01000010, \
    0b01000010, \
    0b01111100, \
    0b01100000, \
    0b01010000, \
    0b01001000, \
    0b01000100, \
  }

#define LETTER_S { \
    0b00111110,  \
    0b01000000, \
    0b01000000, \
    0b00111100, \
    0b00000010, \
    0b00000010, \
    0b00000010, \
    0b00111100, \
  }

#define LETTER_T { \
    0b00111110,  \
    0b00001000, \
    0b00001000, \
    0b00001000, \
    0b00001000, \
    0b00001000, \
    0b00001000, \
    0b00001000, \
  }

#define LETTER_U { \
    0b01000010,  \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b00111100, \
  }

#define LETTER_V { \
    0b01000010,  \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b00100100, \
    0b00011000, \
  }

#define LETTER_W { \
    0b01000010,  \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b01011010, \
    0b01100110, \
    0b01000010, \
  }

#define LETTER_X { \
    0b01000010,  \
    0b01000010, \
    0b00100100, \
    0b00011000, \
    0b00011000, \
    0b00100100, \
    0b01000010, \
    0b01000010, \
  }

#define LETTER_Y { \
    0b01000010,  \
    0b00100100, \
    0b00011000, \
    0b00011000, \
    0b00011000, \
    0b00011000, \
    0b00011000, \
    0b00011000, \
  }

#define LETTER_Z { \
    0b01111110,  \
    0b00000010, \
    0b00000100, \
    0b00001000, \
    0b00010000, \
    0b00100000, \
    0b01000000, \
    0b01111110, \
  }


#define START_CHAR_LED_MATRIX ' '	// Ascii for group separator
#define END_CHAR_LED_MATRIX '~'
const byte alphabet[31][8] = { 	LETTER_A, LETTER_B, LETTER_C, LETTER_D, LETTER_E, LETTER_F, LETTER_G,\
								LETTER_H, LETTER_I, LETTER_J, LETTER_K, LETTER_L, LETTER_M, LETTER_N,\
								LETTER_O, LETTER_P, LETTER_Q, LETTER_R, LETTER_S, LETTER_T, LETTER_U,\
								LETTER_V, LETTER_W, LETTER_X, LETTER_Y, LETTER_Z, SPACE, APOSTROPHE, HEART1, HEART2, HEART3};

const byte charWidth[31] = {8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,3,8,8,8,8};



//***********************
// EEPROM configuration *
//************************

// IP Display
#define EEPROM_START_ADDRESS SERIALTALKS_UUID_ADDRESS+SERIALTALKS_UUID_LENGTH
#define EEPROM_IPDISPLAY_START_ADDRESS EEPROM_START_ADDRESS
#define EEPROM_IPDISPLAY_LENGTH END_CHAR-START_CHAR+1
// LED Matrix
#define EEPROM_LEDMATRIX_DATA_START_ADDRESS EEPROM_IPDISPLAY_START_ADDRESS+EEPROM_IPDISPLAY_LENGTH
#define EEPROM_LEDMATRIX_DATA_LENGTH	((END_CHAR_LED_MATRIX-START_CHAR_LED_MATRIX+1)*8)
#define EEPROM_LEDMATRIX_WIDTH_START_ADDRESS EEPROM_LEDMATRIX_DATA_START_ADDRESS + EEPROM_LEDMATRIX_DATA_LENGTH
#define EEPROM_LEDMATRIX_WITDH_LENGTH END_CHAR_LED_MATRIX-START_CHAR_LED_MATRIX+1
#define EEPROM_LEDMATRIX1_MESSAGE_SAVED_START_ADDRESS EEPROM_LEDMATRIX_WIDTH_START_ADDRESS + EEPROM_LEDMATRIX_WITDH_LENGTH
#define EEPROM_LEDMATRIX1_MESSAGE_SAVED_LENGTH 8
#define EEPROM_LEDMATRIX2_MESSAGE_SAVED_START_ADDRESS EEPROM_LEDMATRIX1_MESSAGE_SAVED_START_ADDRESS + EEPROM_LEDMATRIX1_MESSAGE_SAVED_LENGTH
#define EEPROM_LEDMATRIX2_MESSAGE_SAVED_LENGTH 8
#define EEPROM_LEDMATRIX3_MESSAGE_SAVED_START_ADDRESS EEPROM_LEDMATRIX2_MESSAGE_SAVED_START_ADDRESS + EEPROM_LEDMATRIX2_MESSAGE_SAVED_LENGTH
#define EEPROM_LEDMATRIX3_MESSAGE_SAVED_LENGTH 8
#define EEPROM_LEDMATRIX1_TIMESTEP_START_ADDRESS EEPROM_LEDMATRIX3_MESSAGE_SAVED_START_ADDRESS + EEPROM_LEDMATRIX3_MESSAGE_SAVED_LENGTH
#define EEPROM_LEDMATRIX1_TIMESTEP_LENGTH sizeof(int)
#define EEPROM_LEDMATRIX2_TIMESTEP_START_ADDRESS EEPROM_LEDMATRIX1_TIMESTEP_START_ADDRESS + EEPROM_LEDMATRIX1_TIMESTEP_LENGTH
#define EEPROM_LEDMATRIX2_TIMESTEP_LENGTH sizeof(int)
#define EEPROM_LEDMATRIX3_TIMESTEP_START_ADDRESS EEPROM_LEDMATRIX2_TIMESTEP_START_ADDRESS + EEPROM_LEDMATRIX2_TIMESTEP_LENGTH
#define EEPROM_LEDMATRIX3_TIMESTEP_LENGTH sizeof(int)

#endif