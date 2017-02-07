#include "UltrasonicSensor.h"
#include <arduino.h>
#define TEMPORISATION 50000

void UltrasonicSensor::trig() {
  digitalWrite(m_trig, HIGH);
  delayMicroseconds(20);
  digitalWrite(m_trig, LOW);
  m_ready = false;
}

bool UltrasonicSensor::getReady(){
  return m_ready;
}

void UltrasonicSensor::update() {
  if (handler->getflag()) {
    
    m_mesure = (handler->getEndTime() - handler->getStartTime()) * 340 / 2000;
    handler->setflag(false);  
  }
  else if((handler->getEndTime() - micros()) >= TEMPORISATION){
    m_ready = true;
  }
}

int UltrasonicSensor::getMesure() {
  return m_mesure;
}

void UltrasonicSensor::attach(int trig, int echo)
{
  if (echo == 2) {
    handler = &handler2;
  }
  else if (echo == 3) {
    handler = &handler3;
  } 
  handler->enabler();
  m_trig = trig;
  pinMode(m_trig, OUTPUT);
}
