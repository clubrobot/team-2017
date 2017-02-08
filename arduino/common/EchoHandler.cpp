#include "EchoHandler.h"
#include <Arduino.h>

EchoHandler handler2(2);
EchoHandler handler3(3);

EchoHandler::EchoHandler(int echo)
  : m_echoP(echo) {
  m_enable = 0;
  m_startTime = 0;
  m_endTime = 0;
}

unsigned long EchoHandler::getEndTime() {
  return m_endTime;
}

unsigned long EchoHandler::getStartTime() {
  return m_startTime;
}

bool EchoHandler::getflag() {
  return m_flag;
}

int EchoHandler::getPin() {
  return m_echoP;
}

void EchoHandler::enabler() {
  if (m_enable == 0) {
    m_enable +=1;
    pinMode(m_echoP, INPUT);
    if (m_echoP == 2) {
      attachInterrupt(digitalPinToInterrupt(2), EchoHandler::echoInterrupt_2, CHANGE);
    }
    if (m_echoP == 3) {
      attachInterrupt(digitalPinToInterrupt(3), EchoHandler::echoInterrupt_3, CHANGE);
    }
  }
}

void EchoHandler::echoInterrupt_2() {
  if (digitalRead(2) == HIGH)
  {
    handler2.m_startTime = micros();
  }
  else if (digitalRead(2) == LOW)
  {
    handler2.m_endTime = micros();
    handler2.m_flag = true;
  }
}

void EchoHandler::echoInterrupt_3() {
  if (digitalRead(3) == HIGH)
  {
    handler3.m_startTime = micros();
  }
  else if (digitalRead(3) == LOW)
  {
    handler3.m_endTime = micros();
    handler3.m_flag = true;
  }
}

void EchoHandler::setflag(bool b){
  m_flag = b;
}

