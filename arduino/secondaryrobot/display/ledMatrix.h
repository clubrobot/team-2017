#include <Arduino.h>
#include "../../common/PeriodicProcess.h"
#include "configuration.h"

#ifndef LEDMATRIX_H
#define LEDMATRIX_H
class LedMatrix : public PeriodicProcess
{

  public:
    void attach(byte dataPin, byte clockPin, byte latchPin);			// Attach a matrix to its pin
	void updateMatrix();												// Send data to the registers
	void initMatrix();													// Init the matrix data + update data into registers
	void clearPattern();												// Clear pattern to display array
	void setPattern(int nbPattern);										// Set the pattern nbPattern into the pattern to display array
    //void computeBuffer(char buffer[], byte shift);

  private:

    byte _DATAPIN;					// Matrix register data pin
    byte _CLOCKPIN;					// Matrix register clock pin
    byte _LATCHPIN;					// Matrix register latch pin
	int _data;						// Data to send to the registers
	int _maskColumns;				// Mask to identify Columns into _data
	byte _actualColumn;				// Column to be updated
	byte _patternToDisplay[8];		// Pattern to display = {Row1,Row2,...,Row8}
	byte _patterns[NB_PATTERNS_MAX][8];			// Array of all the message to display

    virtual void process(float timestep);
    void display();
};


#endif //LEDMATRIX_H