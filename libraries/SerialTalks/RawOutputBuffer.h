#ifndef __RAWOUTPUTBUFFER_H__
#define __RAWOUTPUTBUFFER_H__

#include "Buffer.h"

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <string>
typedef std::string String;
#endif


template <int MAX_SIZE>
class RawOutputBuffer : public Buffer<MAX_SIZE>
{
public:

	RawOutputBuffer() : Buffer<MAX_SIZE>(){}
	template <int OTHER_MAX_SIZE>
	RawOutputBuffer(const Buffer<OTHER_MAX_SIZE>& buffer) : Buffer<MAX_SIZE>(buffer){}

	template<typename T>
	bool add(const T& data)
	{
		const byte* bytes; int offset;
		RawDataHelper<T>::toBytes(data, bytes, offset);
		return Buffer<MAX_SIZE>::append(bytes, offset);
	}

	template<typename T>
	RawOutputBuffer& operator<<(const T& data)
	{
		add<T>(data);
		return *this;
	}
};

#endif // __RAWOUTPUTBUFFER_H__
