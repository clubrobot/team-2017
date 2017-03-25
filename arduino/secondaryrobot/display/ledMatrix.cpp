#include "ledMatrix.h"


void LedMatrix::attach(byte dataPin, byte clockPin, byte latchPin, int rotation)
{
	_DATAPIN = dataPin;
	_CLOCKPIN = clockPin;
	_LATCHPIN = latchPin;
	_rotation = rotation;
	pinMode(_DATAPIN, OUTPUT);
	pinMode(_CLOCKPIN, OUTPUT);
	pinMode(_LATCHPIN, OUTPUT);
	_actualColumn = 0;
	setShift(1);
	initMatrix();
	_pattern.init();
	_pattern.setTimestep(PATTERN_TIMESTEP);
}

void LedMatrix::enable()
{
	PeriodicProcess::enable();
	_pattern.enable();
}

void LedMatrix::disable()
{
	PeriodicProcess::disable();
	_pattern.disable();
}

void LedMatrix::update()
{
	PeriodicProcess::update();
	_pattern.update();
}

void LedMatrix::setShift(int shift)
{
	_pattern._shift = shift;
}

void LedMatrix::process(float timestep)
{
	_data|=_maskColumns;  // Turn all column off
	_actualColumn++;
	if (_actualColumn >= 8) {
		_actualColumn = 0;
	}
	switch(_rotation){
		case 90 : 				// 90° rotation
					for (int row = 0; row < 8; row++) {
						if (_pattern._patternToDisplay[_actualColumn]&(0x01<<(row))) {
							_data|=rows[row];  // Turn on this led
						}
						else {
							_data&=~(rows[row]); // Turn off this led
						}
					}
					break;
		case 180 :				// 180° rotation
					for (int row = 0; row < 8; row++) {
						if (_pattern._patternToDisplay[7-row]&(0x01<<(_actualColumn))) {
							_data|=rows[row];  // Turn on this led
						}
						else {
							_data&=~(rows[row]); // Turn off this led
						}
					}
					break;
		case 270 : 				// 270° rotation
					for (int row = 0; row < 8; row++) {
						if (_pattern._patternToDisplay[7-_actualColumn]&(0x01<<(7-row))) {
							_data|=rows[row];  // Turn on this led
						}
						else {
							_data&=~(rows[row]); // Turn off this led
						}
					}
					break;
		default :				// 0° rotation 
					for (int row = 0; row < 8; row++) {
						if (_pattern._patternToDisplay[row]&(0x01<<(7-_actualColumn))) {
							_data|=rows[row];  // Turn on this led
						}
						else {
							_data&=~(rows[row]); // Turn off this led
						}
					}

	}
	_data&= ~(cols[_actualColumn]); // Turn whole column on at once
	updateMatrix();	
}


void LedMatrix::updateMatrix()
{
	byte octet1 = _data&0x00FF;
	byte octet2 = (_data&0xFF00)>>8;
	digitalWrite(_LATCHPIN,LOW);
	shiftOut(_DATAPIN,_CLOCKPIN,MSBFIRST,octet1);
	shiftOut(_DATAPIN,_CLOCKPIN,MSBFIRST,octet2);
	digitalWrite(_LATCHPIN,HIGH);
}


void LedMatrix::initMatrix() 
{
	//col à 0 pour allumer row à 1 pour allumer
	_data = 0;
	for(int i = 0;i<8;i++){
		_data+=cols[i];
	}
	_maskColumns = _data;
	updateMatrix();
}

void LedMatrix::computeBuffer(char buffer[])
{
	talks.out << "nouveau message : " << buffer;
	_pattern.clearPatterns();
	int i;
	for (i = 0; buffer[i]!='\0' && i < NB_PATTERNS_MAX; i++) {
		for (int j = 0; j < 8; j++) {
			if (buffer[i] == ' ') {
				_pattern._patterns[i][j] = alphabet[26][j];
			} else if (buffer[i] == '\'') {
				_pattern._patterns[i][j] = alphabet[27][j];
			} else {
				_pattern._patterns[i][j] = alphabet[buffer[i] - 97][j];
			}
		}
	}
	_pattern._nbPatterns = i;
}

void Pattern::init()
{
	_endOfPreviousPattern = 7;
	_currentPattern = 0;
	_nbPatterns = 9;
	clearPatternToDisplay();
}

void Pattern::clearPatternToDisplay() 
{
	// Clear pattern to display array
	for (int i = 0; i < 8; i++) {
		_patternToDisplay[i] = 0;
	}
}

void Pattern::clearPatterns()
{
	// Clear patterns message
	for (int i = 0; i<NB_PATTERNS_MAX; i++){
		for (int j = 0; j<8; j++){
			_patterns[i][j] = 0;
		}
	}
}


void Pattern::setPattern()
{
	for (int i = 0; i < 8; i++) {
		_patternToDisplay[i] = _patterns[_currentPattern][i] ;
	}
}


void Pattern::slidePattern() 
{
	setTimestep(PATTERN_TIMESTEP*_shift);
	if(_endOfPreviousPattern-(8-_patternWidth[_currentPattern])<0){
		_currentPattern = ++_currentPattern % (_nbPatterns);
		_endOfPreviousPattern = 7;
	}
	_endOfPreviousPattern-=_shift;	
    for (int row = 0; row < 8; row++) {
        _patternToDisplay[row] = _patternToDisplay[row]<<_shift;
		for(int i = 0; i< _shift; i++){
			if ((_patterns[_currentPattern][row] & 0x01 <<  (_endOfPreviousPattern+i+1)%8) >= 1) {
				_patternToDisplay[row] |= 0x01<<i ;
    		}
		} 
    }
}

void Pattern::process(float timestep)
{
	slidePattern();
}

