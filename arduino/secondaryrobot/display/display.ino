#include <Arduino.h>
#include "../../common/SerialTalks.h"
#include "IPDisplay.h" 
#include "instructions.h"
#include "configuration.h"
#include "ledMatrix.h"

// Variables

Ipdisplay ipdisplay;
LedMatrix ledmatrix1;
LedMatrix ledmatrix2;
LedMatrix ledmatrix3;

void setup()
{
	Serial.begin(SERIALTALKS_BAUDRATE);
	talks.begin(Serial);

	talks.bind(SET_MATRIX_MESSAGE_OPCODE, SET_MATRIX_MESSAGE);
	talks.bind(SET_IPDISPLAY_MESSAGE_OPCODE, SET_IPDISPLAY_MESSAGE);
	talks.bind(SET_EEPROM_CHAR_IPDISPLAY_OPCODE,SET_EEPROM_CHAR_IPDISPLAY);
	talks.bind(SET_EEPROM_SPEED_MATRIX_OPCODE,SET_EEPROM_SPEED_MATRIX);
	talks.bind(SET_EEPROM_CHAR_LEDMATRIX_OPCODE,SET_EEPROM_CHAR_LEDMATRIX);

	// Variables initialisation

	ipdisplay.attach(DATA_IPDISPLAY, CLOCK_IPDISPLAY, LATCH_IPDISPLAY);
	ipdisplay.setTimestep(IP_DISPLAY_TIMESTEP);
	ipdisplay.enable();

	ledmatrix1.attach(DATA_MATRIX1,CLOCK_MATRIX1,LATCH_MATRIX1,ROTATION_MATRIX_1,1);
	ledmatrix1.setTimestep(LED_MATRIX_TIMESTEP);
	ledmatrix1.enable();

	

}

void loop()
{
	talks.execute();
	ipdisplay.update();
	ledmatrix1.update();
	//printEEPROM();
	/*talks.out << "EEPROM_LEDMATRIX_DATA_START_ADDRESS : " << EEPROM_LEDMATRIX_DATA_START_ADDRESS << "\n";
	talks.out << "#\n";
	for (int i = 0; i<8;i++){
		talks.out << EEPROM.read(EEPROM_LEDMATRIX_DATA_START_ADDRESS + i + 8*( '#' - START_CHAR_LED_MATRIX)) << "\n";
	}
	talks.out << EEPROM.read(EEPROM_LEDMATRIX_WIDTH_START_ADDRESS +  '#' - START_CHAR_LED_MATRIX) << "\n\n";

	talks.out << "%\n";
	for (int i = 0; i<8;i++){
		talks.out << EEPROM.read(EEPROM_LEDMATRIX_DATA_START_ADDRESS + i + 8*( '%' - START_CHAR_LED_MATRIX)) << "\n";
	}
	talks.out << EEPROM.read(EEPROM_LEDMATRIX_WIDTH_START_ADDRESS +  '%' - START_CHAR_LED_MATRIX) << "\n\n";

	talks.out << "}\n";
	for (int i = 0; i<8;i++){
		talks.out << EEPROM.read(EEPROM_LEDMATRIX_DATA_START_ADDRESS + i + 8*( '}' - START_CHAR_LED_MATRIX)) << "\n";
	}
	talks.out << EEPROM.read(EEPROM_LEDMATRIX_WIDTH_START_ADDRESS +  '}' - START_CHAR_LED_MATRIX) << "\n\n";
	//talks.out << EEPROM_LEDMATRIX1_MESSAGE_SAVED_START_ADDRESS << "\n";*/
}
