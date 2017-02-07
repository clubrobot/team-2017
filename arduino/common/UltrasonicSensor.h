#ifndef __ULTRASONICSENSOR_H__
#define __ULTRASONICSENSOR_H__

#include "EchoHandler.h"

class UltrasonicSensor {
  private :
    int m_mesure;
    EchoHandler* handler;
    int m_trig;
    bool m_ready;

  public :
    void attach(int trig, int echo);
    void update();
    int getMesure();
    void trig();
    bool getReady();


};
#endif