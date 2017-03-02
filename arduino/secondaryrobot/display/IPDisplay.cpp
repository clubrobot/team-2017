#include <Arduino.h>
#include "IPDisplay.h"
#include "configuration.h"
#include "../../common/SerialTalks.h"
#include <EEPROM.h>

extern SerialTalks talks;

void Ipdisplay::attach(byte dataPin, byte clockPin, byte latchPin)
{
	clearDisplay();
	m_DATA = dataPin;
	m_CLOCK = clockPin;
	m_LATCH = latchPin;
	m_disp_number = 0;

	// sets the pins as output
	pinMode(m_DATA, OUTPUT);
	pinMode(m_CLOCK, OUTPUT);
	pinMode(m_LATCH, OUTPUT);

	for (int i = 0; i < DISP_NUMBER; i++)
	{
		pinMode(DISP_PIN[i], OUTPUT);
		digitalWrite(DISP_PIN[i], LOW);
	}
}

void Ipdisplay::process(float timestep)
{
	digitalWrite(DISP_PIN[m_disp_number], LOW); // Turn the off the previous 7 seg display
	m_disp_number++;
	if (m_disp_number >= 3)
		m_disp_number = 0;

	digitalWrite(LATCH_IPDISPLAY, LOW);
	for (int i = 3; i >= 0; i--)
	{
		shiftOut(DATA_IPDISPLAY, CLOCK_IPDISPLAY, MSBFIRST, m_toSend[i][m_disp_number]);
	}
	digitalWrite(LATCH_IPDISPLAY, HIGH);
	digitalWrite(DISP_PIN[m_disp_number], HIGH); // Turn on the next 7 seg display
}

void Ipdisplay::clearDisplay()
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			m_toSend[i][j] = 0;
		}
	}
}

void Ipdisplay::computeBuffer(char buffer[], byte shift)
{
	int cpt = 0;
	for (int display = 0; display < 4; display++)
	{
		for (int i = 0; i < 3; i++)
		{
			m_toSend[display][i] = 0;
			if ((display)*3 + i + 1 > shift)
			{
				if ((buffer[cpt] >= START_CHAR) && (buffer[cpt] <= END_CHAR))
				{
					for (int j = 0; j < 8; j++)
					{
						m_toSend[display][i] += (((EEPROM.read(EEPROM_IPDISPLAY_START_ADDRESS+buffer[cpt]-START_CHAR)) & (0x01 << j)) / (0x01 << j)) * DISP[display][7 - j];
					}
				}

				cpt++;

				if (buffer[cpt] == '.' && buffer[cpt-1] != '.')
				{
					m_toSend[display][i] += DISP[display][7];
					cpt++;
				}
			}
		}
	}
}
