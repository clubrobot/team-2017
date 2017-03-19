#include <Arduino.h>
#include "../../common/SerialTalks.h"
#include "IPDisplay.h" 
#include "instructions.h"
#include "configuration.h"
#include "ledMatrix.h"

// Variables

Ipdisplay ipdisplay;
LedMatrix ledmatrix;

void setup()
{
	Serial.begin(SERIALTALKS_BAUDRATE);
	talks.begin(Serial);

	talks.bind(SET_MATRIX_MESSAGE_OPCODE, SET_MATRIX_MESSAGE);
	talks.bind(SET_IPDISPLAY_MESSAGE_OPCODE, SET_IPDISPLAY_MESSAGE);
	#ifdef INIT_EEPROM
		talks.bind(INIT_EEPROM_DATA_OPCODE, INIT_EEPROM_DATA);
	#endif

	// Variables initialisation

	ipdisplay.attach(DATA_IPDISPLAY, CLOCK_IPDISPLAY, LATCH_IPDISPLAY);
	ipdisplay.setTimestep(IP_DISPLAY_TIMESTEP);
	ipdisplay.enable();

	ledmatrix.attach(DATA_MATRIX1,CLOCK_MATRIX1,LATCH_MATRIX1);
	ledmatrix.setTimestep(LED_MATRIX_TIMESTEP);
	ledmatrix.enable();

}

void loop()
{
	talks.execute();
	ipdisplay.update();
	ledmatrix.update();
}
