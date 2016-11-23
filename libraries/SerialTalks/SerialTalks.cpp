#include <EEPROM.h>
#include "SerialTalks.h"


// Global instance

SerialTalks talks;


// Built-in instructions

static bool getUUIDInstruction(Deserializer& input, Serializer& output)
{
	char uuid[SERIALTALKS_UUID_LENGTH];
	talks.getUUID(uuid);
	output << uuid;
	return true;
}

static bool setUUIDInstruction(Deserializer& input, Serializer& output)
{	
	char uuid[SERIALTALKS_UUID_LENGTH];
	input >> uuid;
    talks.setUUID(uuid);
    return false;
}


// SerialTalks

SerialTalks::SerialTalks()
{
	// Initialize UUID stuff
	char uuid[SERIALTALKS_UUID_LENGTH];
	if (!getUUID(uuid) || uuid[0] == '\0')
	{
		generateRandomUUID(uuid, SERIALTALKS_DEFAULT_UUID_LENGTH);
		setUUID(uuid);
	}

	// Add UUID accessors
	attach(SERIALTALKS_GETUUID_OPCODE, getUUIDInstruction);
	attach(SERIALTALKS_SETUUID_OPCODE, setUUIDInstruction);
}

int SerialTalks::send(byte* buffer, int size)
{
	int count = 0;
	count += m_stream->write(SERIALTALKS_SLAVE_BYTE);
	count += m_stream->write(byte(size));
	count += m_stream->write(buffer, size);
	return count;
}

void SerialTalks::attach(byte opcode, Instruction instruction)
{
	// Add a command to execute when receiving the specified opcode
	if (opcode < SERIALTALKS_MAX_OPCODE)
		m_instructions[opcode] = instruction;
}

bool SerialTalks::execute(byte opcode, byte* inputBuffer)
{
	if (m_instructions[opcode] != 0)
	{
		m_outputBuffer[0] = opcode;

		Deserializer input (inputBuffer);
		Serializer   output(m_outputBuffer + 1);

		if (m_instructions[opcode](input, output))
			send(m_outputBuffer, output.buffer - m_outputBuffer);
		return true;
	}
	return false;
}

bool SerialTalks::execute()
{
	bool ret = false;
	int length = m_stream->available();
	for (int i = 0; i < length; i++)
	{
		// Read the incoming byte
		byte inc = byte(m_stream->read());
		
		// Use a state machine to process the above byte
		switch (m_state)
		{
		// An instruction always begin with the Master byte
		case SERIALTALKS_WAITING_STATE:
			if (inc == SERIALTALKS_MASTER_BYTE)
				m_state = SERIALTALKS_INSTRUCTION_STARTING_STATE;
			continue;

		// The second byte is the instruction size (for example: 'R', '\x02', '\x03', '\x31')
		case SERIALTALKS_INSTRUCTION_STARTING_STATE:
			m_bytesNumber  = inc;
			m_bytesCounter = 0;
			m_state = (m_bytesNumber < SERIALTALKS_INPUT_BUFFER_SIZE)
			?	SERIALTALKS_INSTRUCTION_RECEIVING_STATE
			:	SERIALTALKS_WAITING_STATE;
			continue;

		// The first instruction byte is the opcode and the others the parameters
		case SERIALTALKS_INSTRUCTION_RECEIVING_STATE:
			m_inputBuffer[m_bytesCounter++] = inc;
			if (m_bytesCounter >= m_bytesNumber)
			{
				byte opcode = m_inputBuffer[0];
				ret |= execute(opcode, m_inputBuffer + 1);
				m_state = SERIALTALKS_WAITING_STATE;
			}
		}
	}
	return ret;
}

bool SerialTalks::getUUID(char* uuid)
{
	for (int i = 0; i < int(EEPROM.length()); i++)
	{
		uuid[i] = EEPROM.read(SERIALTALKS_UUID_ADDRESS + i);
		switch(byte(uuid[i]))
		{
		case '\0': return true;
		case 0xFF: return false;
		default: continue;
		}
	}
	return false;
}

void SerialTalks::setUUID(const char* uuid)
{
	int i = 0;
	do
		EEPROM.write(SERIALTALKS_UUID_ADDRESS + i, uuid[i]);
	while(uuid[i++] != '\0');
}

void SerialTalks::generateRandomUUID(char* uuid, int length)
{
	// Initialize the random number generator
	randomSeed(analogRead(0));

	// Generate the UUID from a list of random hexadecimal numbers
	for (int i = 0; i < length; i++)
	{
		if (i % 5 == 4)
			uuid[i] = '-';
		else
		{
			long digit = random(16);
			if (digit < 10)
				uuid[i] = char('0' + digit);
			else
				uuid[i] = char('a' + digit - 10);
		}
	}
}
