#ifndef __SERIALTALKS_H__
#define __SERIALTALKS_H__

#include <Arduino.h>

#ifndef SERIALTALKS_BAUDRATE
#define SERIALTALKS_BAUDRATE 115200
#endif

#ifndef SERIALTALKS_INPUT_BUFFER_SIZE
#define SERIALTALKS_INPUT_BUFFER_SIZE 64
#endif

#ifndef SERIALTALKS_OUTPUT_BUFFER_SIZE
#define SERIALTALKS_OUTPUT_BUFFER_SIZE 64
#endif

#ifndef SERIALTALKS_UUID_ADDRESS
#define SERIALTALKS_UUID_ADDRESS 0x0000000000
#endif

#ifndef SERIALTALKS_UUID_LENGTH
#define SERIALTALKS_UUID_LENGTH	32
#endif

#ifndef SERIALTALKS_MAX_OPCODE
#define SERIALTALKS_MAX_OPCODE 0x10
#endif

#define SERIALTALKS_MASTER_BYTE 'R'
#define SERIALTALKS_SLAVE_BYTE  'A'

#define SERIALTALKS_DEFAULT_UUID_LENGTH 9

#define SERIALTALKS_GETUUID_OPCODE 0x0
#define SERIALTALKS_SETUUID_OPCODE 0x1
#define SERIALTALKS_STDOUT_OPCODE  0xFF
#define SERIALTALKS_STDERR_OPCODE  0xFE

#include "SerialUtils.h"


class SerialTalks
{
public: // Public API

	typedef bool (*Instruction)(Deserializer& input, Serializer& output);

	SerialTalks();

	template<typename GenericSerial> void begin(GenericSerial& serial)
	{
		serial.begin(SERIALTALKS_BAUDRATE);
		m_stream = &serial;
		
		// Tell it's ready by sending the UUID
		execute(SERIALTALKS_GETUUID_OPCODE, 0);
	}

	void attach(byte opcode, Instruction instruction);

	bool execute(byte opcode, byte* inputBuffer);
	bool execute();

	bool getUUID(char* uuid);
	void setUUID(const char* uuid);

	static void generateRandomUUID(char* uuid, int length);

protected: // Protected methods

	int send(byte* buffer, int size);

	// Attributes

	Stream*     m_stream;

	Instruction	m_instructions[SERIALTALKS_MAX_OPCODE];

	byte        m_inputBuffer [SERIALTALKS_INPUT_BUFFER_SIZE];
	byte        m_outputBuffer[SERIALTALKS_OUTPUT_BUFFER_SIZE];

	enum //     m_state
	{
		SERIALTALKS_WAITING_STATE,
		SERIALTALKS_INSTRUCTION_STARTING_STATE,
		SERIALTALKS_INSTRUCTION_RECEIVING_STATE,
	} m_state;
	
	int	m_bytesNumber;
	int m_bytesCounter;
};

extern SerialTalks talks;

#endif // __SERIALTALKS_H__
