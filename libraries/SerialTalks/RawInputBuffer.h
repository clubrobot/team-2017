#ifndef __RAWINPUTBUFFER_H__
#define __RAWINPUTBUFFER_H__

#include "Buffer.h"

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <string>
typedef std::string String;
#endif

template <int MAX_SIZE>
class RawInputBuffer : public Buffer<MAX_SIZE>
{
public:
	RawInputBuffer()                                     : Buffer<MAX_SIZE>(),       m_cursor(0){}
	template <int OTHER_MAX_SIZE>
	RawInputBuffer(const Buffer<OTHER_MAX_SIZE>& buffer) : Buffer<MAX_SIZE>(buffer), m_cursor(0){}

	int available() const {return Buffer<MAX_SIZE>::m_size - m_cursor;}

	template<typename T>
	RawInputBuffer& operator>>(T& data)
	{
		if (available() >= sizeof(T))
			data = *(T*)(Buffer<MAX_SIZE>::m_bytes + m_cursor);
		m_cursor += sizeof(T);
		return *this;
	}

	RawInputBuffer operator>>(String& data)
	{
		data = String((char*)(Buffer<MAX_SIZE>::m_bytes + m_cursor));
		m_cursor += data.length() + 1;
		return *this;
	}

protected:

	int m_cursor;
};

#endif // __RAWINPUTBUFFER_H__
