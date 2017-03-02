#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "../../common/SerialTalks.h"

//************************
// initialisation EEPROM *
//************************

#define INIT_EEPROM				// disable to save ROM space



//************
// Constants *
//************


// Time constants
#define IP_DISPLAY_TIMESTEP 3e-3    // Execution step time in s

// cIP_display constants
#define IP_DISPLAY_BUFFER_SIZE 25   // Size of the data buffer receiver
#define DISP_NUMBER 3               // Number of 7 seg common pin





//****************************
// IP Display pin definition *
//****************************

// common pin DISP1 , DISP2 , DISP3
const byte DISP_PIN[] = {2, 3, 4};

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
const byte DISP[4][8] = {{128, 1, 8, 32, 64, 2, 4, 16},
                         {128, 1, 4, 16, 32, 64, 2, 8},
                         {128, 1, 8, 32, 64, 2, 4, 16},
                         {128, 1, 4, 16, 32, 64, 2, 8}};

//******************************
// LED Matrix 1 pin definition *
//******************************

// shift register pins
#define DATA_MATRIX1 8
#define CLOCK_MATRIX1 9
#define LATCH_MATRIX1 10

//******************************
// LED Matrix 2 pin definition *
//******************************

// shift register pins
#define DATA_MATRIX2
#define CLOCK_MATRIX2
#define LATCH_MATRIX2

//******************************
// LED Matrix 3 pin definition *
//******************************

// shift register pins
#define DATA_MATRIX3
#define CLOCK_MATRIX3
#define LATCH_MATRIX3

//***********************
// EEPROM configuration *
//************************


#define EEPROM_START_ADDRESS SERIALTALKS_UUID_ADDRESS+SERIALTALKS_UUID_LENGTH
#define EEPROM_IPDISPLAY_START_ADDRESS EEPROM_START_ADDRESS
#define EEPROM_IPDISPLAY_LENGTH END_CHAR-START_CHAR+1
#define EEPROM_LEDMATRIX_START_ADDRESS EEPROM_IPDISPLAY_START_ADDRESS+EEPROM_IPDISPLAY_LENGTH
#define EEPROM_LEDMATRIX_LENGTH		//TODO








#endif