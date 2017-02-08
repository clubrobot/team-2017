#include <Arduino.h>
#include "../../common/SerialTalks.h"
#include "IPDisplay.h"
#include "pin.h"
#include "instructions.h"

#define IP_DISPLAY_TIMESTEP 1e-3 //s

// Variables

Ipdisplay ipdisplay;

void setup()
{
  Serial.begin(SERIALTALKS_BAUDRATE);
  talks.begin(Serial);

  talks.bind(SET_MATRIX_MESSAGE_OPCODE, SET_MATRIX_MESSAGE);
  talks.bind(SET_IPDISPLAY_MESSAGE_OPCODE, SET_IPDISPLAY_MESSAGE);

  // Variables initialisation

  ipdisplay.attach(DATA_IPDISPLAY, CLOCK_IPDISPLAY, LATCH_IPDISPLAY);
  ipdisplay.setTimestep(IP_DISPLAY_TIMESTEP);
}

void loop()
{
  talks.execute();
  ipdisplay.update();
}
