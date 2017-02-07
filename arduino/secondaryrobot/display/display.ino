#include <Arduino.h>

//#include <FrequencyTimer2.h>
#include "pinmap.h"
#include "IPDisplay.h"
#include "pin.h"


#define DISP_NUMBER 3


#define MAXVAL 25

// Prototypes
void clearValue() ;
void clearDisplay();
void display();


// Variables

byte value[MAXVAL] ;
byte toSend[4][3] =  {{79, 230, 98}, {112, 245, 0}, {0, 0, 0}, {0, 0, 0}};

byte disp_number = 0;  // Number of the effective 7 seg display




void setup() {

  Serial.begin(9600);

  clearValue();

  // sets the pins as output

  pinMode(DATA_IPDISPLAY, OUTPUT);
  pinMode(CLOCK_IPDISPLAY, OUTPUT);
  pinMode(LATCH_IPDISPLAY, OUTPUT);
  for (int i = 0; i < DISP_NUMBER; i++) {
    pinMode(DISP_PIN[i], OUTPUT);
    digitalWrite(DISP_PIN[i], LOW);
  }


/*
  // Turn off toggling of pin 11
  FrequencyTimer2::disable();
  // Set refresh rate (interrupt timeout period)
  FrequencyTimer2::setPeriod(2000);
  // Set interrupt routine to be called
  FrequencyTimer2::setOnOverflow(display);
*/



}

void loop() {

  if (Serial.available() > 0) {
    delay(100);
    int cpt = 0;
    clearValue();
    Serial.println();
    //Serial.print("nouveau message : ");
    while (Serial.available() > 0) {
      value[cpt] = Serial.read();
      //Serial.println(value[cpt]);
      value[cpt] -= '0';
      cpt++;
    }
    cpt = 0;
    clearDisplay();
    int nbDigits = 0;
    int decalage = 0;
    for (int i = 0; i < MAXVAL; i++) {
      if (value[cpt] >= 0 && value[cpt] < ('9' - '0')) {
        nbDigits++;
      }
      cpt++;
    }
    decalage = (12 - nbDigits) / 2;
    cpt=0;
    Serial.println(decalage);

    for (int display = 0; display < 4; display++) {
      for (int i = 0; i < 3; i++) {
        toSend[display][i] = 0;
        if ((display) *3 + i+1 > decalage) {

          //Serial.println(value[cpt]);
          if (value[cpt] >= 0 && value[cpt] < ('9' - '0')) {
            for (int j = 0; j < 8; j++) {
              toSend[display][i] += ((segToDisplay[value[cpt]] & (0x01 << j)) / (0x01 << j)) * DISP[display][7 - j];
            }
          }
          //Serial.println(toSend[display][i]);
          cpt++;

          if (value[cpt] == 254) {
            //Serial.println(value[cpt]);
            toSend[display][i] +=  DISP[display][7];
            cpt++;
          }
        }
      }
    }
  }
}


// Interrupt routine
void display() {
  digitalWrite(DISP_PIN[disp_number], LOW); // Turn the off the previous 7 seg display
  disp_number++;
  if (disp_number >= 3)
    disp_number = 0;

  digitalWrite(LATCH_IPDISPLAY, LOW);
  for (int i = 3; i >= 0; i--) {
    shiftOut(DATA_IPDISPLAY, CLOCK_IPDISPLAY, MSBFIRST, toSend[i][disp_number]);
  }
  digitalWrite(LATCH_IPDISPLAY, HIGH);
  digitalWrite(DISP_PIN[disp_number], HIGH); // Turn on the next 7 seg display
}

void clearDisplay() {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 3; j++) {
      toSend[i][j] = 0;
    }
  }
}

void clearValue() {
  for (int i = 0; i < MAXVAL; i++) {
    value[i] = -1;
  }
}
