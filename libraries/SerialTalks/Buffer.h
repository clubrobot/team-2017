#ifndef __BUFFER_H__
#define __BUFFER_H__

#ifdef ARDUINO
#include <Arduino.h>
#else
typedef unsigned char byte;
#endif // ARDUINO

template<int MAX_SIZE>
class Buffer
{
public:

	Buffer() : m_size(0){}

	const byte* asArray() const {return m_bytes;}
	int         getSize() const {return m_size;}
	bool        isEmpty() const {return m_size <= 0;}

	bool resize(int size)
	{
		m_size = size;
		if (m_size < 0)
		{
			m_size = 0;
			return false;
		}
		else if (m_size > MAX_SIZE)
		{
			m_size = MAX_SIZE;
			return false;
		}
		return true;
	}

	bool append(byte data)
	{
		if (resize(m_size + 1))
		{
			m_bytes[m_size - 1] = data;
			return true;
		}
		return false;
	}

	bool append(const byte data[], int size)
	{
		for (int i = 0; i < size; i++)
		{
			if (!append(data[i]))
				return false;
		}
		return true;
	}

	byte& operator[](int i)
	{
		if (i % m_size >= 0)
			return m_bytes[i % m_size];	
		else
			return m_bytes[i % m_size + m_size];
	}

	const byte& operator[](int i) const
	{
		if (i % m_size >= 0)
			return m_bytes[i % m_size];	
		else
			return m_bytes[i % m_size + m_size];
	}

protected:

	byte m_bytes[MAX_SIZE];
	int  m_size;
};

#endif // __BUFFER_H__
