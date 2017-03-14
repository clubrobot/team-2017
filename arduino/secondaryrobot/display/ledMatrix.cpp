#include "ledMatrix.h"







void LedMatrix::attach(byte dataPin, byte clockPin, byte latchPin)
{
	_DATAPIN = dataPin;
	_CLOCKPIN = clockPin;
	_LATCHPIN = latchPin;
}

void LedMatrix::process(float timestep)
{
	_data|=_maskColumns;  // Turn all column off
	_actualColumn++;
	if (_actualColumn == 8) {
		_actualColumn = 0;
	}
	for (int row = 0; row < 8; row++) {
		if (_patternToDisplay[row]&(0x01<<(7-_actualColumn))) {
			_data|=rows[row];  // Turn on this led
		}
		else {
			_data&=~(rows[row]); // Turn off this led
		}
	}
	_data&= ~(cols[_actualColumn]); // Turn whole column on at once (for equal lighting times)
	this -> updateMatrix();	
}


void LedMatrix::updateMatrix(){
  byte octet1 = _data&0x00FF;
  byte octet2 = (_data&0xFF00)>>8;
  digitalWrite(_LATCHPIN,LOW);
  shiftOut(_DATAPIN,_CLOCKPIN,MSBFIRST,octet1);
  shiftOut(_DATAPIN,_CLOCKPIN,MSBFIRST,octet2);
  digitalWrite(_LATCHPIN,HIGH);
}


void LedMatrix::initMatrix() {
  //col à 0 pour allumer row à 1 pour allumer
  _data = 0;
  for(int i = 0;i<8;i++){
    _data+=cols[i];
  }
  this->updateMatrix();
}


void LedMatrix::clearPattern() {
  // Clear pattern to display array
  for (int i = 0; i < 8; i++) {
    _patternToDisplay[i] = 0;
  }
}


void LedMatrix::setPattern(int nbPattern) {
  for (int i = 0; i < 8; i++) {
    _patternToDisplay[i] = _patterns[nbPattern][i] ;
  }
}


void LedMatrix::slidePattern(int nbPattern, int del) {
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
}
