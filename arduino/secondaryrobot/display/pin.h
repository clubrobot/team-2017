#ifndef PIN_H
#define PIN_H

//*************
// IP Display *
//*************

// common pin DISP1 , DISP2 , DISP3
const byte DISP_PIN[] = {2, 3, 4};

// shift register pins
#define DATA_IPDISPLAY 5
#define CLOCK_IPDISPLAY 6
#define LATCH_IPDISPLAY 7

//ASCII TABLE => starts at @32
//a,b,c,d,e,f,g,dp   ASCII    dec
//                               0     ,     1     ,     2     ,     3     ,     4     ,     5     ,     6     ,     7     ,     8     ,     9
const byte segToDisplay[] = {0b11111100, 0b01100000, 0b11011010, 0b11110010, 0b01100110, 0b10110110, 0b10111110, 0b11100000, 0b11111110, 0b11110110};

//Weight of each segment
//                         a , b , c , d , e , f , g , dp
const byte DISP[4][8] = {{128, 1, 8, 32, 64, 2, 4, 16},
                         {128, 1, 4, 16, 32, 64, 2, 8},
                         {128, 1, 8, 32, 64, 2, 4, 16},
                         {128, 1, 4, 16, 32, 64, 2, 8}};

//***************
// LED Matrix 1 *
//***************

// shift register pins
#define DATA_MATRIX1 8
#define CLOCK_MATRIX1 9
#define LATCH_MATRIX1 10

//***************
// LED Matrix 2 *
//***************

// shift register pins
#define DATA_MATRIX2
#define CLOCK_MATRIX2
#define LATCH_MATRIX2

//***************
// LED Matrix 3 *
//***************

// shift register pins
#define DATA_MATRIX3
#define CLOCK_MATRIX3
#define LATCH_MATRIX3

#endif