#ifndef PIN_H
#define PIN_H

//*************
// IP Display *
//*************

// common pin         DISP1 , DISP2 , DISP3
const byte DISP_PIN[] {  2  ,   3   ,   4   };

// shift register pins
#define DATA_IPDISPLAY  5
#define CLOCK_IPDISPLAY 6
#define LATCH_IPDISPLAY 7

//***************
// LED Matrix 1 *
//***************

// shift register pins
#define DATA_MATRIX1  8
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