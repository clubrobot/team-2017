#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "../../common/SerialTalks.h"

//************************
// initialisation EEPROM *
//************************

#define INIT_EEPROM				// disable to save ROM space
#define INIT_EEPROM_LED_MATRIX			// disable to save ROM space



//************
// Constants *
//************


// Time constants
#define IP_DISPLAY_TIMESTEP 3e-3    // Execution step time in s

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
#ifdef INIT_EEPROM
//                            SPACE    ,     !     ,     "     ,     #     ,     $     ,     %     ,     &     ,     '     ,     (     ,     )
const byte segToDisplay[] = {0b00000000, 0b01100001, 0b01000100, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b01000000, 0b10011100, 0b11110000,
//                               *     ,     +     ,     ,     ,     -     ,     .     ,     /     ,     0     ,     1     ,     2     ,     3
							 0b00000000, 0b00000000, 0b00000000, 0b00000010, 0b00000001, 0b00000000, 0b11111100, 0b01100000, 0b11011010, 0b11110010,
//                               4     ,     5     ,     6     ,     7     ,     8     ,     9     ,     :     ,     ;     ,     <     ,     =
							 0b01100110, 0b10110110, 0b10111110, 0b11100000, 0b11111110, 0b11110110, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
//                               >     ,     ?     ,     @     ,     A     ,     B     ,     C     ,     D     ,     E     ,     F     ,     G
							 0b00000000, 0b00000000, 0b00000000, 0b11101110, 0b00111110, 0b10011100, 0b01111010, 0b10011110, 0b10001110, 0b10111100,
//                               H     ,     I     ,     J     ,     K     ,     L     ,     M     ,     N     ,     O     ,     P     ,     Q
							 0b01101110, 0b01100000, 0b01110000, 0b00000000, 0b00011100, 0b00000000, 0b00000000, 0b11111100, 0b11001110, 0b00000000,
//                               R     ,     S     ,     T     ,     U     ,     V     ,     W     ,     X     ,     Y     ,     Z     ,     [
						 	 0b11101110, 0b10110110, 0b00000000, 0b01111100, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b10011100,
//                               \     ,     ]     ,     ^     ,     _     ,     `     ,     a     ,     b     ,     c     ,     d     ,     e
						 	 0b00000000, 0b11110000, 0b00000000, 0b00010000, 0b00000000, 0b11101110, 0b00111110, 0b00011010, 0b01111010, 0b10011110,
//                               f     ,     g     ,     h     ,     i     ,     j     ,     k     ,     l     ,     m     ,     n     ,     o
						 	 0b10001110, 0b10111100, 0b00101110, 0b00100000, 0b01110000, 0b00000000, 0b00011100, 0b00000000, 0b00101010, 0b00111010,
//                               p     ,     q     ,     r     ,     s     ,     t     ,     u     ,     v     ,     w     ,     x     ,     y
						 	 0b11001110, 0b11100110, 0b00001010, 0b10110110, 0b00011110, 0b00111000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
//                               z     ,     {     ,     |     ,     }     ,     ~     
						 	 0b00000000, 0b00000000, 0b01100000, 0b00000000, 0b00000000};
#endif

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

// generapl configuration

#define NB_PATTERNS_MAX 20

// Weight of each dot
const unsigned int rows[] = {4, 16384, 8, 32768, 128, 1, 512, 16}; //Row1,Row2,...,Row8
const unsigned int cols[] = {256, 2048, 64, 2, 32, 4096, 8192, 1024}; //Col1,Col2,...,Col8

// Alphabet
#ifdef INIT_EEPROM_LED_MATRIX

#define START_CHAR_LED_MATRIX 0x1D	// Ascii for group separator
#define END_CHAR_LED_MATRIX '~'
const byte alphabet[32][8] = { A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z, SPACE, APOSTROPHE, HEART1, HEART2, HEART3, SPACE};

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
#endif	// INIT_EEPROM_LED_MATRIX

//***********************
// EEPROM configuration *
//************************

// IP Display
#define EEPROM_START_ADDRESS SERIALTALKS_UUID_ADDRESS+SERIALTALKS_UUID_LENGTH
#define EEPROM_IPDISPLAY_START_ADDRESS EEPROM_START_ADDRESS
#define EEPROM_IPDISPLAY_LENGTH END_CHAR-START_CHAR+1
// LED Matrix
#define EEPROM_LEDMATRIX_START_ADDRESS EEPROM_IPDISPLAY_START_ADDRESS+EEPROM_IPDISPLAY_LENGTH
#define EEPROM_LEDMATRIX_LENGTH		//TODO

#endif