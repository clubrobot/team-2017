#include <Arduino.h>
#include "../../common/PeriodicProcess.h"
#include "configuration.h"
#include "IPDisplay.h"		// DEBUG

#ifndef LEDMATRIX_H
#define LEDMATRIX_H

extern Ipdisplay ipdisplay;

class Pattern : public PeriodicProcess
{
	friend class LedMatrix;

public:

	void init();								// init the pattern
	void clearPatternToDisplay();				// Clear pattern to display array
	void clearPatterns();						// Clear _patterns array
	void setPattern();							// Set the current pattern into the patternToDisplay array
	void slidePattern();						// Slide the patternToDisplay array


private:

	byte _patternToDisplay[8];					// Pattern to display = {Row1,Row2,...,Row8}
	byte _patterns[NB_PATTERNS_MAX][8];			// Array of all the message to display
	int _currentPattern;						// The position of the current pattern in the array _patterns
	int _endOfPreviousPattern;					// Separation between 2 consecutive patterns being sliding
	byte _nbPatterns;							// Number of patterns having to be displayed
	byte _shift;								// Number of shifts to apply when sliding patterns
	byte _patternWidth[NB_PATTERNS_MAX];		// The width of each pattern

	Pattern() : _patterns {LETTER_B,LETTER_O,LETTER_R,LETTER_N,LETTER_I,LETTER_B,LETTER_U,LETTER_S,SPACE}, _patternWidth {7,4,7,7,8,8,8,8,2} {}

	virtual void process(float timestep);
	
};


class LedMatrix : public PeriodicProcess
{

  public:

    void attach(byte dataPin, byte clockPin, byte latchPin, int rotation);			// Attach a matrix to its pin
	void updateMatrix();															// Send data to the registers
	void initMatrix();																// Init the matrix data + update data into registers
	void computeBuffer(char buffer[]);												// Compute Serial input
	void enable();
	void disable();
	void update();
	void setShift(int shift);		// Set the shift parameter for the pattern

  private:

    byte _DATAPIN;					// Matrix register data pin
    byte _CLOCKPIN;					// Matrix register clock pin
    byte _LATCHPIN;					// Matrix register latch pin
	int _data;						// Data to send to the registers
	int _maskColumns;				// Mask to identify Columns into _data
	byte _actualColumn;				// Column to be updated
	Pattern _pattern;
	int _rotation;					// rotation of the matrix {0,90,180,270}
	

    virtual void process(float timestep);
    void display();
};



#endif //LEDMATRIX_H