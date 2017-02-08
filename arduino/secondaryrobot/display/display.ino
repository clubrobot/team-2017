#include <Arduino.h>
#include "../../common/SerialTalks.h"
#include "IPDisplay.h"
#include "instructions.h"
#include "configuration.h"

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
