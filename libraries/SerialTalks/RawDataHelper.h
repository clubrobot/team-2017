#ifndef __RAWDATAHELPER_H__
#define __RAWDATAHELPER_H__

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <string>
typedef std::string String;
#endif


template<typename T>
struct RawDataHelper
{
	static void fromBytes(const byte* bytes, T& data, int& size)
	{
		data = *(T*)(bytes);
		size = sizeof(T);
	}

	static void toBytes(const T& data, const byte*& bytes, int& size)
	{
		bytes = (const byte*)(&data);
		size = sizeof(T);
	}
};

template<>
struct RawDataHelper<String>
{
	static void fromBytes(const byte bytes[], String& data, int& size)
	{
		data = String((char*)(bytes));
		size = data.length() + 1;
	}
	
	static void toBytes(const String& data, const byte*& bytes, int& size)
	{
		bytes = (const byte*)(data.c_str());
		size = data.length() + 1;
	}
};

#endif // __RAWDATAHELPER_H__
