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


// OutputStack

template<typename T>
OutputStack& operator<<(OutputStack& stack, const T& data)
{
	byte* address = (byte*)(&data);
	stack.append(address, sizeof(data));
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
	count += Serial.write(SERIALTALKS_STARTBYTE);
	count += Serial.write(opcode);
	count += Serial.write(byte(1 + stack.m_length));
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
	/*
    // Read incoming commands
    int serialLength = Serial.available();
    for (int i = 0; i < serialLength; i++)
    {
        // Append the incoming byte to the current command
        byte newByte = byte(Serial.read());
        if (newByte == ROBOTCOM_COMMAND_START_BYTE || inputBufferLength > 0)
            inputBuffer[inputBufferLength++] = newByte;
        
        // Execute it if this is the end of the current command
        if (inputBufferLength >= 2)
        {
            byte* commandBuffer = inputBuffer + 2;
            int commandLength = inputBuffer[1];
            if (commandLength <= inputBufferLength - 2)
            {
                if (commandLength > 0)
                {
                    byte opcode = commandBuffer[0];
                    int outputLength = 0;
                    outputBuffer[0] = opcode;
                    if (commandsList[opcode] != NULL)
                        outputLength = commandsList[opcode](commandLength, commandBuffer, outputBuffer);
                    if (outputLength > 0)
                        send(outputLength, outputBuffer);
                }
                inputBufferLength = 0;
            }
        }
    }*/
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
