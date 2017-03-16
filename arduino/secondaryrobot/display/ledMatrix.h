#include <Arduino.h>
#include "../../common/PeriodicProcess.h"
#include "configuration.h"

#ifndef LEDMATRIX_H
#define LEDMATRIX_H

class Pattern : public PeriodicProcess
{
	friend class LedMatrix;

public:

	void init();													// init the pattern
	void clearPattern();												// Clear pattern to display array
	void setPattern();										// Set the current pattern into the patternToDisplay array
	void slidePattern();												// Slide the patternToDisplay array


private:

	byte _patternToDisplay[8];					// Pattern to display = {Row1,Row2,...,Row8}
	byte _patterns[NB_PATTERNS_MAX][8];			// Array of all the message to display
	int _currentPattern;						// The position of the current pattern in the array _patterns
	int _endOfPreviousPattern;					// Separation between 2 consecutive patterns being sliding

	virtual void process(float timestep);
	
};


class LedMatrix : public PeriodicProcess
{

  public:

    void attach(byte dataPin, byte clockPin, byte latchPin);			// Attach a matrix to its pin
	void updateMatrix();												// Send data to the registers
	void initMatrix();													// Init the matrix data + update data into registers
    //void computeBuffer(char buffer[], byte shift);

  private:

    byte _DATAPIN;					// Matrix register data pin
    byte _CLOCKPIN;					// Matrix register clock pin
    byte _LATCHPIN;					// Matrix register latch pin
	int _data;						// Data to send to the registers
	int _maskColumns;				// Mask to identify Columns into _data
	byte _actualColumn;				// Column to be updated
	Pattern _pattern;
	

    virtual void process(float timestep);
    void display();
};



#endif //LEDMATRIX_H