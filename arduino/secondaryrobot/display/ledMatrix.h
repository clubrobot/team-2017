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
    //void computeBuffer(char buffer[], byte shift);

  private:

    byte _DATAPIN;
    byte _CLOCKPIN;
    byte _LATCHPIN;
	int _data;			// Data to send to the registers

    virtual void process(float timestep);
    void display();
};


#endif //LEDMATRIX_H