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

#define SERIALTALKS_CONNECT_OPCODE 0x0
#define SERIALTALKS_GETUUID_OPCODE 0x1
#define SERIALTALKS_SETUUID_OPCODE 0x2
#define SERIALTALKS_STDOUT_OPCODE  0xFF
#define SERIALTALKS_STDERR_OPCODE  0xFE

#include "SerialUtils.h"


class SerialTalks
{
public: // Public API

	class ostream : public Print
	{
	public:

		virtual size_t write(uint8_t);
		virtual size_t write(const uint8_t *buffer, size_t size);

		template<typename T> ostream& operator<<(const T& object)
		{
			print(object);
			return *this;
		}
	
	protected:

		ostream(SerialTalks& parent, byte opcode);

		SerialTalks& m_parent;
		const byte   m_opcode;

		friend class SerialTalks;
	};

	typedef bool (*Instruction)(SerialTalks& inst, Deserializer& input, Serializer& output);

	SerialTalks();

	bool isConnected() const;

	void connect();

	template<typename GenericSerial> void begin(GenericSerial& serial)
	{
		serial.begin(SERIALTALKS_BAUDRATE);
		m_stream = &serial;
		connect();
	}

	void bind(byte opcode, Instruction instruction);

	bool execinstruction(byte opcode, byte* inputBuffer);
	bool execute();

	bool getUUID(char* uuid);
	void setUUID(const char* uuid);

	static void generateRandomUUID(char* uuid, int length);

	// Public attributes (yes we dare!)

	ostream     out;
	ostream     err;

protected: // Protected methods

	int send(byte opcode, const byte* buffer, int size);

	// Attributes

	Stream*     m_stream;
	bool		m_connected;

	Instruction	m_instructions[SERIALTALKS_MAX_OPCODE];

	byte        m_inputBuffer [SERIALTALKS_INPUT_BUFFER_SIZE];
	byte        m_outputBuffer[SERIALTALKS_OUTPUT_BUFFER_SIZE];

	enum //     m_state
	{
		SERIALTALKS_WAITING_STATE,
		SERIALTALKS_INSTRUCTION_STARTING_STATE,
		SERIALTALKS_INSTRUCTION_RECEIVING_STATE,
	}           m_state;
	
	byte        m_bytesNumber;
	byte        m_bytesCounter;

private:

	static bool connectInstruction(SerialTalks& inst, Deserializer& input, Serializer& output);
	static bool getUUIDInstruction(SerialTalks& inst, Deserializer& input, Serializer& output);
	static bool setUUIDInstruction(SerialTalks& inst, Deserializer& input, Serializer& output);
};

extern SerialTalks talks;

#endif // __SERIALTALKS_H__
