#ifndef __RAWINPUTBUFFER_H__
#define __RAWINPUTBUFFER_H__

#include "Buffer.h"
#include "RawDataHelper.h"


template <int MAX_SIZE>
class RawInputBuffer : public Buffer<MAX_SIZE>
{
public:

	RawInputBuffer() : Buffer<MAX_SIZE>(), m_cursor(0){}
	template <int OTHER_MAX_SIZE>
	RawInputBuffer(const Buffer<OTHER_MAX_SIZE>& buffer) : Buffer<MAX_SIZE>(buffer), m_cursor(0){}

	int available() const {return Buffer<MAX_SIZE>::m_size - m_cursor;}

	template<typename T>
	const T next()
	{
		T data; int offset;
		RawDataHelper<T>::fromBytes(Buffer<MAX_SIZE>::m_bytes + m_cursor, data, offset);
		m_cursor += offset;
		return data;
	}

	template<typename T>
	RawInputBuffer& operator>>(T& data)
	{
		data = next<T>();
		return *this;
	}

protected:

	int m_cursor;
};

#endif // __RAWINPUTBUFFER_H__
