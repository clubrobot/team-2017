#ifndef __RAWINPUTBUFFER_H__
#define __RAWINPUTBUFFER_H__

#include "Buffer.h"

template <int MAX_SIZE>
class RawInputBuffer : public Buffer<MAX_SIZE>
{
public:
	RawInputBuffer() : m_cursor(0){}

	int available() const {return Buffer<MAX_SIZE>::m_size - m_cursor;}

	template<typename T>
	RawInputBuffer& operator>>(T& data)
	{
		if (available() >= sizeof(T))
			data = *(T*)(Buffer<MAX_SIZE>::m_bytes + m_cursor);
		m_cursor += sizeof(T);
		return *this;
	}
/*
	template <>
	operator>>(String& data)
	{

	}
*/
protected:

	int m_cursor;
};

#endif // __RAWINPUTBUFFER_H__
