#ifndef __SERIALTALKS_H__
#define __SERIALTALKS_H__

#include <Arduino.h>

#define SERIALTALKS_BAUDRATE		115200
#define SERIALTALKS_MASTER_BYTE		'R'
#define SERIALTALKS_SLAVE_BYTE		'A'

#define SERIALTALKS_BUFFER_SIZE		64

#define SERIALTALKS_UUID_ADDRESS	0x0000000000
#define SERIALTALKS_UUID_LENGTH		9

#define SERIALTALKS_GETUUID_OPCODE	0x0
#define SERIALTALKS_SETUUID_OPCODE	0x1
#define SERIALTALKS_MAX_OPCODE		0x10

#define SERIALTALKS_STDOUT_OPCODE	0xFF
#define SERIALTALKS_STDERR_OPCODE	0xFE


class Stack
{
public:

	Stack() : m_length(0){}

	/* */ byte& operator[](int i);
	const byte& operator[](int i) const;

	const byte*	getBytes() const;
	int			getLength() const;
	bool 		isEmpty() const;

	bool append(byte data);
	bool append(const byte* buffer, int length);

	void shift(int n);

	void clear();

protected:

	byte	m_buffer[SERIALTALKS_BUFFER_SIZE];
	int		m_length;
};

class InputStack : public Stack
{
public:

	InputStack()					: Stack(), m_cursor(0){}
	InputStack(const Stack& stack)	: Stack(stack), m_cursor(0){}

	template<typename T> friend InputStack& operator>>(InputStack& stack, T& data);

protected:

	int		m_cursor;
};

template<typename T> InputStack& operator>>(InputStack& stack, T& data)
{
	data = *(T*)(stack.m_buffer + stack.m_cursor++);
	return stack;
}

template<>
InputStack& operator>> <String>(InputStack& stack, String& data);

class OutputStack : public Stack
{
public:

	OutputStack()					: Stack(){}
	OutputStack(const Stack& stack)	: Stack(stack){}

	template<typename T> friend OutputStack& operator<<(OutputStack& stack, const T& data);
};

template<typename T> OutputStack& operator<<(OutputStack& stack, const T& data)
{
	stack.append((const byte*)(&data), sizeof(data));
	return stack;
}

template<> OutputStack& operator<< <String>(OutputStack& stack, const String& data);

class SerialTalks_
{
public:

	typedef bool (*Instruction)(InputStack& input, OutputStack& output);

	SerialTalks_();

	void connect(byte opcode, Instruction instruction);
	bool execute();

	String getUUID();
	void setUUID(String uuid);

private: // Private methods

	int send(byte opcode, Stack& stack);

	String generateRandomUUID();

	// Attributes

	enum
	{
		SERIALTALKS_WAITING_STATE,
		SERIALTALKS_INSTRUCTION_STARTING_STATE,
		SERIALTALKS_INSTRUCTION_RECEIVING_STATE,
	}	m_state;
	int	m_instructionBytesCounter;

	Instruction	m_instructions[SERIALTALKS_MAX_OPCODE];

	InputStack	m_inputBuffer;
	OutputStack	m_outputBuffer;
};

extern SerialTalks_ SerialTalks;

#endif // __SERIALTALKS_H__
