#include <arduino.h>
#ifndef __ECHOHANDLER_H__
#define __ECHOHANDLER_H__

class EchoHandler {

  private :

    int m_echoP;
    unsigned long m_startTime;
    unsigned long m_endTime;
    bool m_flag;
    static void echoInterrupt_2();
    static void echoInterrupt_3(); 
    int m_enable; 

  public :

    EchoHandler(int echo);
    unsigned long getEndTime();
    unsigned long getStartTime();
    bool getflag();
    int getPin();
    void enabler();
    void setflag(bool b);
};

extern EchoHandler handler2;
extern EchoHandler handler3;

#endif

