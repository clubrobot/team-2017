#include <Arduino.h>
#include <EEPROM.h>

#include "SerialTalks.h"


// Built-in instructions

bool SerialTalks::getUUIDInstruction(InputStack &input, OutputStack& output)
{
	String uuid = getUUID();
	output << uuid;
	return true;
}

bool SerialTalks::setUUIDInstruction(InputStack &input, OutputStack& output)
{	
    String uuid;
	input >> uuid;
    setUUID(uuid);
    return false;
}


// Stack

byte& Stack::operator[](int i)
{
	if (i % m_length >= 0)
		return m_buffer[i % m_length];	
	else
		return m_buffer[i % m_length + m_length];
}

const byte& Stack::operator[](int i) const
{
	if (i % m_length >= 0)
		return m_buffer[i % m_length];	
	else
		return m_buffer[i % m_length + m_length];
}

int Stack::getLength() const
{
	return m_length;
}

bool Stack::isEmpty() const
{
	return (m_length <= 0);
}

bool Stack::append(byte data)
{
	if (m_length < SERIALTALKS_BUFFER_SIZE)
	{
		m_buffer[m_length++] = data;
		return true;
	}
	return false;
}

bool Stack::append(byte buffer[], int length)
{
	for (int i = 0; i < length; i++)
	{
		if (!append(buffer[i]))
			return false;
	}
	return true;
}

void Stack::shift(int n)
{
	if (n > m_length)
	{
		clear();
	}
	else if (n > 0)
	{
		for (int i = 0; i < m_length - n; i++)
		{
			m_buffer[i] = m_buffer[i + n];
		}
		m_length -= n;
	}
}

void Stack::clear()
{
	m_length = 0;
}


// InputStack

template<typename T>
InputStack& operator>>(InputStack& stack, T& data)
{
	byte* address = (byte*)(&data);
	for (int i = 0; i < sizeof(data); i++)
	{
		if (stack.m_cursor < stack.m_length)
			address[i] = stack.m_buffer[stack.m_cursor++];
	}
	return stack;
}

template<>
InputStack& operator>><String>(InputStack& stack, String& data)
{
	data = String((char*)(&stack.m_buffer[stack.m_cursor]));
	stack.m_cursor += data.length() + 1;
	return stack;
}


// OutputStack

template<typename T>
OutputStack& operator<<(OutputStack& stack, const T& data)
{
	byte* address = (byte*)(&data);
	stack.append(address, sizeof(data));
	return stack;
}

template<>
OutputStack& operator<<(OutputStack& stack, const String& data)
{
	stack.append(data.c_str(), data.length() + 1);
	return stack;
}


// SerialTalks

SerialTalks::SerialTalks()
{
	String uuid = getUUID();
	if (uuid == "")
		uuid = generateRandomUUID();
	
	constructor(uuid);
}

SerialTalks::SerialTalks(String uuid)
{
	constructor(uuid);
}

void SerialTalks::constructor(String uuid)
{
	// Initialize serial port
	Serial.begin(SERIALTALKS_BAUDRATE);

	// Initialize UUID stuff
	setUUID(uuid);

	// Add UUID accessors
	addInstruction(SERIALTALKS_GETUUID_OPCODE, getUUIDInstruction);
	addInstruction(SERIALTALKS_SETUUID_OPCODE, setUUIDInstruction);

	// Tell it's ready by sending the UUID
	OutputStack out;
	out << getUUID();
	send(SERIALTALKS_GETUUID_OPCODE, out);
}

int SerialTalks::send(byte opcode, Stack& stack)
{
	int count = 0;
	count += Serial.write(SERIALTALKS_SLAVE_BYTE);
	count += Serial.write(byte(1 + stack.m_length));
	count += Serial.write(opcode);
	count += Serial.write(stack.m_buffer, stack.m_length);
	return count;
}

void SerialTalks::connect(byte opcode, Instruction instruction)
{
	// Add a command to execute when receiving the specified opcode
	if (opcode < SERIALTALKS_MAX_OPCODE)
		m_instructions[opcode] = instruction;
}

void SerialTalks::execute()
{
	int length = Serial.available();
	for (int i = 0; i < length; i++)
	{
		// Read the incoming byte
		byte inc = byte(Serial.read());
		
		// Use a state machine to process the above byte
		switch (m_state)
		{
		// An instruction always begin with the Master byte
		case SERIALTALKS_WAITING_STATE:
			if (inc == SERIALTALKS_MASTER_BYTE)
			{
				m_inputBuffer.clear();
				m_outputBuffer.clear();
				m_state = SERIALTALKS_INSTRUCTION_STARTING_STATE;
			}
			continue;

		// The second byte is the instruction size (for example: 'R', '\x02', '\x03', '\x31')
		case SERIALTALKS_INSTRUCTION_STARTING_STATE:
			m_instructionBytesCounter = inc;
			m_state = (m_byteCounter > 0 && m_byteCounter < SERIALTALKS_BUFFER_SIZE)
			?	SERIALTALKS_INSTRUCTION_RECEIVING_STATE
			:	SERIALTALKS_WAITING_STATE;
			continue;

		// The first instruction byte is the opcode and the others the parameters
		case SERIALTALKS_INSTRUCTION_RECEIVING_STATE:
			m_inputBuffer.append(inc);
			m_instructionBytesCounter--;
			if (m_instructionBytesCounter == 0)
			{
				byte opcode;
				m_inputBuffer >> opcode;
				if (m_instructions[opcode] != 0)
				{
					if (m_instructions[opcode](m_inputBuffer, m_outputBuffer))
						send(opcode, m_outputBuffer);
				}
				m_state = SERIALTALKS_WAITING_STATE;
			}
		}
	}
}

String SerialTalks::getUUID()
{
	String uuid = "";
	for (int i = 0; i < int(EEPROM.length()); i++)
	{
		char ch = EEPROM.read(SERIALTALKS_UUID_ADDRESS + i);
		if (ch != '\0' && ch != 0xFF)
			uuid += ch;
		else
			return uuid;
	}
	return "";
}

String SerialTalks::generateRandomUUID()
{
	// Initialize the random number generator
	randomSeed(analogRead(0));

	// Generate the UUID from a list of random hexadecimal numbers
	String uuid = "";
	for (int i = 0; i < SERIALTALKS_UUID_LENGTH; i++)
	{
		if (i % 5 == 4)
			uuid += '-';
		else
			uuid += String(random(16), HEX);
	}
	return uuid;
}

void SerialTalks::setUUID(String uuid)
{
	int uuidLength = uuid.length();
	for (int i = 0; i < uuidLength; i++)
	{
		char ch = uuid.charAt(i);
		EEPROM.write(SERIALTALKS_UUID_ADDRESS + i, ch);
	}
	EEPROM.write(SERIALTALKS_UUID_ADDRESS + uuidLength, '\0');
}
