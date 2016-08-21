/*
 * Show messages on an 8x8 led matrix,
 * scrolling from right to left.
 *
 * Uses FrequencyTimer2 library to
 * constantly run an interrupt routine
 * at a specified frequency. This
 * refreshes the display without the
 * main loop having to do anything.
 *
 */

#include <FrequencyTimer2.h>

//alphabet position letters
//#define a 0
//#define b 1
//#define c 2
//#define d 3
//#define e 4
//#define f 5
//#define g 6
//#define h 7
//#define i 8
//#define j 9
//#define k 10
//#define l 11
//#define m 12
//#define n 13
//#define o 14
//#define p 15
//#define q 16
//#define r 17
//#define s 18
//#define t 19
//#define u 20
//#define v 21
//#define w 22
//#define x 23
//#define y 24
//#define z 25

#define HEART1 { \
    0b01101100,  \
    0b10011110, \
    0b10111110, \
    0b11111110, \
    0b01111100, \
    0b00111000, \
    0b00010000, \
    0b00000000, \
  }

#define HEART2 { \
    0b00000000,  \
    0b00101000, \
    0b01011100, \
    0b01111100, \
    0b00111000, \
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


#define A { \
    0b00011000,  \
    0b00100100, \
    0b01000010, \
    0b01000010, \
    0b01111110, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
  }

#define B { \
    0b01111100,  \
    0b01000010, \
    0b01000010, \
    0b01111100, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b01111100, \
  }

#define C { \
    0b00011110,  \
    0b00100000, \
    0b01000000, \
    0b01000000, \
    0b01000000, \
    0b01000000, \
    0b00100000, \
    0b00011110, \
  }

#define D { \
    0b01111000,  \
    0b01000100, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b01000100, \
    0b01111000, \
  }

#define E  { \
    0b01111110,  \
    0b01000000, \
    0b01000000, \
    0b01111110, \
    0b01000000, \
    0b01000000, \
    0b01000000, \
    0b01111110, \
  }

#define F { \
    0b01111110,  \
    0b01000000, \
    0b01000000, \
    0b01111100, \
    0b01000000, \
    0b01000000, \
    0b01000000, \
    0b01000000, \
  }

#define G { \
    0b00011110,  \
    0b00100000, \
    0b01000000, \
    0b01000000, \
    0b01001110, \
    0b01000010, \
    0b00100010, \
    0b00011100, \
  }

#define H { \
    0b01000010,  \
    0b01000010, \
    0b01000010, \
    0b01111110, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
  }

#define I { \
    0b00111110,  \
    0b00001000, \
    0b00001000, \
    0b00001000, \
    0b00001000, \
    0b00001000, \
    0b00001000, \
    0b00111110,  \
  }

#define J { \
    0b01111110,  \
    0b00001000, \
    0b00001000, \
    0b00001000, \
    0b00001000, \
    0b00001000, \
    0b00001000, \
    0b00110000, \
  }

#define K { \
    0b01000100,  \
    0b01001000, \
    0b01010000, \
    0b01100000, \
    0b01100000, \
    0b01010000, \
    0b01001000, \
    0b01000100, \
  }

#define L { \
    0b01000000,  \
    0b01000000, \
    0b01000000, \
    0b01000000, \
    0b01000000, \
    0b01000000, \
    0b01000000, \
    0b01111110, \
  }

#define M { \
    0b01000010,  \
    0b01100110, \
    0b01011010, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
  }

#define N { \
    0b01000010,  \
    0b01100010, \
    0b01010010, \
    0b01001010, \
    0b01000110, \
    0b01000010, \
    0b01000000, \
    0b01000000, \
  }

#define O { \
    0b00011000,  \
    0b00100100, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b00100100, \
    0b00011000, \
  }

#define P { \
    0b01111100,  \
    0b01000010, \
    0b01000010, \
    0b01111100, \
    0b01000000, \
    0b01000000, \
    0b01000000, \
    0b01000000, \
  }

#define Q { \
    0b00011000,  \
    0b00100100, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b01001010, \
    0b00100100, \
    0b00011010, \
  }

#define R { \
    0b01111100,  \
    0b01000010, \
    0b01000010, \
    0b01111100, \
    0b01100000, \
    0b01010000, \
    0b01001000, \
    0b01000100, \
  }

#define S { \
    0b00111110,  \
    0b01000000, \
    0b01000000, \
    0b00111100, \
    0b00000010, \
    0b00000010, \
    0b00000010, \
    0b00111100, \
  }

#define T { \
    0b00111110,  \
    0b00001000, \
    0b00001000, \
    0b00001000, \
    0b00001000, \
    0b00001000, \
    0b00001000, \
    0b00001000, \
  }

#define U { \
    0b01000010,  \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b00111100, \
  }

#define V { \
    0b01000010,  \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b00100100, \
    0b00011000, \
  }

#define W { \
    0b01000010,  \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b01000010, \
    0b01011010, \
    0b01100110, \
    0b01000010, \
  }

#define X { \
    0b01000010,  \
    0b01000010, \
    0b00100100, \
    0b00011000, \
    0b00011000, \
    0b00100100, \
    0b01000010, \
    0b01000010, \
  }

#define Y { \
    0b01000010,  \
    0b00100100, \
    0b00011000, \
    0b00011000, \
    0b00011000, \
    0b00011000, \
    0b00011000, \
    0b00011000, \
  }

#define Z { \
    0b01111110,  \
    0b00000010, \
    0b00000100, \
    0b00001000, \
    0b00010000, \
    0b00100000, \
    0b01000000, \
    0b01111110, \
  }



byte col = 0;
byte leds[8][8];

// pin[xx] on led matrix connected to nn on Arduino (-1 is dummy to make array start at pos 1)
int pins[17] = { -1, 14, 15, 16, 17, 18, 19, 12, 13, 10, 2, 4, 5, 6, 7, 8, 9};

// col[xx] of leds = pin yy on led matrix
int cols[8] = {pins[9], pins[10], pins[11], pins[12], pins[13], pins[14], pins[15], pins[16]};  //{pins[16], pins[15], pins[14], pins[13], pins[12], pins[11], pins[10], pins[9]};
// row[xx] of leds = pin yy on led matrix
int rows[8] = {pins[8], pins[7], pins[6], pins[5], pins[4], pins[3], pins[2], pins[1]}; //{pins[1], pins[2], pins[3], pins[4], pins[5], pins[6], pins[7], pins[8]};

int numPatterns = 4;
byte patterns[50][8] = {
  HEART1, HEART2, HEART3, HEART2
};

byte alphabet[32][8] = {
  A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z, SPACE, APOSTROPHE, HEART1, HEART2, HEART3, SPACE
};

char readBuffer[50];

int pattern = 0;

void setup() {

  Serial.begin(115200);

  for (int i = 0; i < 10; i++) {
    readBuffer[i] = 0;
  }

  // sets the pins as output
  for (int i = 1; i <= 16; i++) {
    pinMode(pins[i], OUTPUT);
  }

  // set up cols and rows
  for (int i = 1; i <= 8; i++) {
    digitalWrite(cols[i - 1], LOW);
  }

  for (int i = 1; i <= 8; i++) {
    digitalWrite(rows[i - 1], LOW);
  }

  clearLeds();

  // Turn off toggling of pin 11
  FrequencyTimer2::disable();
  // Set refresh rate (interrupt timeout period)
  FrequencyTimer2::setPeriod(2000);
  // Set interrupt routine to be called
  FrequencyTimer2::setOnOverflow(display);

  for (int i = 0; i < 8; i++) {
    if (i % 2 != 0) {
      for (int j = 7; j >= 0; j--) {
        clearLeds();
        leds[i][j] = 1;
        delay(10);
      }
    } else {
      for (int j = 0; j < 8; j++) {
        clearLeds();
        leds[i][j] = 1;
        delay(10);
      }
    }
  }
  for (int i = 7; i >= 0 ; i--) {
    if (i % 2 == 0) {
      for (int j = 7; j >= 0; j--) {
        //clearLeds();
        leds[i][j] = 1;
        delay(10);
      }
    } else {
      for (int j = 0; j < 8; j++) {
        //clearLeds();
        leds[i][j] = 1;
        delay(10);
      }
    }
  }

  clearLeds();

  Serial.println("Ready");

  setPattern(pattern);
  while (!Serial.available()) {

  pattern = ++pattern % numPatterns;
setPattern(pattern);
delay(100);
  }
}

void loop() {



  if (Serial.available() > 0) {
    int cpt = 0;
    Serial.println();
    Serial.print("nouveau message : ");
    while (Serial.available() > 0) {
      readBuffer[cpt] = Serial.read();
      Serial.print(readBuffer[cpt]);
      cpt++;
    }
    numPatterns = cpt + 1;
    for (int i = 0; i < numPatterns; i++) {
      for (int j = 0; j < 8; j++) {
        patterns[i][j] = 0;
      }
    }
    for (int i = 0; i < numPatterns - 1; i++) {
      for (int j = 0; j < 8; j++) {
        if (readBuffer[i] == 32) {
          patterns[i + 1][j] = alphabet[26][j];
        } else if (readBuffer[i] == 39) {
          patterns[i + 1][j] = alphabet[27][j];
        } else {
          patterns[i + 1][j] = alphabet[readBuffer[i] - 97][j];
        }
      }
    }
    pattern = -1;
  }
  pattern = ++pattern % numPatterns;
slidePattern(pattern, 60);
}

void clearLeds() {
  // Clear display array
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      leds[i][j] = 0;
    }
  }
}

void setPattern(int pattern) {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if ((patterns[pattern][i] & 0x01 << (7 - j)) >= 1) {
        leds[i][j] = 1 ;
      } else {
        leds[i][j] = 0 ;
      }

    }

  }
}

void slidePattern(int pattern, int del) {
  for (int l = 0; l < 8; l++) {
    for (int i = 0; i < 7; i++) {
      for (int j = 0; j < 8; j++) {
        leds[j][i] = leds[j][i + 1];
      }
    }
    for (int j = 0; j < 8; j++) {
      if ((patterns[pattern][j] & 0x01 << (8 - l)) >= 1) {
        leds[j][7] = 1 ;
      } else {
        leds[j][7] = 0 ;
      }

    }
    delay(del);
  }
}

// Interrupt routine
void display() {
  digitalWrite(cols[col], LOW);  // Turn whole previous column off
  col++;
  if (col == 8) {
    col = 0;
  }
  for (int row = 0; row < 8; row++) {
    if (leds[col][7 - row] == 1) {
      digitalWrite(rows[row], LOW);  // Turn on this led
    }
    else {
      digitalWrite(rows[row], HIGH); // Turn off this led
    }
  }
  digitalWrite(cols[col], HIGH); // Turn whole column on at once (for equal lighting times)
}
