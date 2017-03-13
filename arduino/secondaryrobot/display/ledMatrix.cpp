#include "ledMatrix.h"







void LedMatrix::attach(byte dataPin, byte clockPin, byte latchPin)
{
	_DATAPIN = dataPin;
	_CLOCKPIN = clockPin;
	_LATCHPIN = latchPin;
}

void LedMatrix::process(float timestep)
{

	
}


void LedMatrix::updateMatrix(){
  byte octet1 = _data&0x00FF;
  byte octet2 = (_data&0xFF00)>>8;
  digitalWrite(_LATCHPIN,LOW);
  shiftOut(_DATAPIN,_CLOCKPIN,MSBFIRST,octet1);
  shiftOut(_DATAPIN,_CLOCKPIN,MSBFIRST,octet2);
  digitalWrite(_LATCHPIN,HIGH);
}

/*
int initMatrix(int dataPin, int clockPin, int latchPin) {
  //col à 0 pour allumer row à 1 pour allumer
  unsigned int data = 0;
  for(int i = 0;i<8;i++){
    data+=cols[i];
  }
  updateMatrix(dataPin,clockPin,latchPin,data);
  return data;
}

void display() {
  dataMatrix|=maskCols;  // Turn all column off
  col++;
  if (col == 8) {
    col = 0;
  }
  for (int row = 0; row < 8; row++) {
    if (leds[row]&(0x01<<(7-col))) {
      dataMatrix|=rows[row];  // Turn on this led
    }
    else {
       dataMatrix&=~(rows[row]); // Turn off this led
    }
  }
  dataMatrix&= ~(cols[col]); // Turn whole column on at once (for equal lighting times)
  updateMatrix(DATA,CLOCK,LATCH,dataMatrix);
}

void clearLeds() {
  // Clear display array
  for (int i = 0; i < 8; i++) {
    leds[i] = 0;
  }
}

void setPattern(int pattern) {
  for (int i = 0; i < 8; i++) {
    leds[i] = patterns[pattern][i] ;
  }
}

void slidePattern(int pattern, int del) {
  for (int l = 0; l < 8; l++) {
    for (int i = 0; i < 8; i++) {
        leds[i] = leds[i]<<1;
    }
    for (int j = 0; j < 8; j++) {
      if ((patterns[pattern][j] & 0x01 << (8 - l)) >= 1) {
        leds[j] |= 1 ;
      } 
    }
    delay(del);
  }
}*/
