#include <Arduino.h>
#include "../../common/PeriodicProcess.h"
#include "configuration.h"

#ifndef LEDMATRIX_H
#define LEDMATRIX_H
class LedMatrix : public PeriodicProcess
{

  public:
    void attach(byte dataPin, byte clockPin, byte latchPin);
    //void computeBuffer(char buffer[], byte shift);

  private:

    byte _DATA;
    byte _CLOCK;
    byte _LATCH;

    virtual void process(float timestep);
    void display();
};


#endif //LEDMATRIX_H