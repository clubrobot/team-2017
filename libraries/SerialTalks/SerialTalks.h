#ifndef __SERIALTALKS_H__
#define __SERIALTALKS_H__

#define SERIALTALKS_BAUDRATE		115200
#define SERIALTALKS_STARTBYTE		'A'
#define SERIALTALKS_STOPBYTE		'Z'

#define SERIALTALKS_BUFFER_SIZE		64

#define SERIALTALKS_UUID_ADDRESS	0x0000000000
#define SERIALTALKS_UUID_LENGTH		9

#define SERIALTALKS_LOG_OPCODE		0xFF
#define SERIALTALKS_GETUUID_OPCODE	0x0
#define SERIALTALKS_SETUUID_OPCODE	0x1
#define SERIALTALKS_MAX_OPCODE		0x10


class Stack
{
public:

	Stack() : m_length(0){}

	/* */ byte& operator[](int i);
	const byte& operator[](int i) const;

	int getLength() const;

	bool append(byte data);
	bool append(byte buffer[], int length);

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

	template<typename T>
	friend InputStack& operator>>(InputStack& stack, T& data);

protected:

	int		m_cursor;
};

class OutputStack : public Stack
{
public:

	OutputStack()					: Stack(){}
	OutputStack(const Stack& stack)	: Stack(stack){}

	template<typename T>
	friend OutputStack& operator<<(OutputStack& stack, const T& data);
};

class SerialTalks
{
public:

	typedef bool (*Instruction)(InputStack& input, OutputStack& output);

	SerialTalks();
	SerialTalks(String uuid);

	void connect(byte opcode, Instruction instruction);

	bool execute();

	static String getUUID();
	static void setUUID(String uuid);

private:

	void constructor(String uuid);

	static int send(byte opcode, Stack& stack);

	static String generateRandomUUID();

	Instruction	m_instructions[COMMANDS_MAX_OPCODE];
};

#endif // __SERIALTALKS_H__
