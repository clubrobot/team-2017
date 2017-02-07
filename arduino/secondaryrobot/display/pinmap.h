#ifndef PINMAP_H
#define PINMAP_H


//ASCII TABLE => starts at @32
 //a,b,c,d,e,f,g,dp   ASCII    dec
 //                             0     ,     1     ,     2     ,     3     ,     4     ,     5     ,     6     ,     7     ,     8     ,     9 
const byte segToDisplay[] { 0b11111100, 0b01100000, 0b11011010, 0b11110010, 0b01100110, 0b10110110, 0b10111110, 0b11100000, 0b11111110, 0b11110110};

//Weight of each segment
//                         a , b , c , d , e , f , g , dp
const byte DISP[4][8]   {{128, 1 , 8 , 32, 64, 2 , 4 , 16},\
                         {128, 1 , 4 , 16, 32, 64, 2 , 8 },\
                         {128, 1 , 8 , 32, 64, 2 , 4 , 16},\
                         {128, 1 , 4 , 16, 32, 64, 2 , 8 }};

#endif //PINMAP_H
