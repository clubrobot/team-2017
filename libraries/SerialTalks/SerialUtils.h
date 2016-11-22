#ifndef __SERIALUTILS_H__
#define __SERIALUTILS_H__

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <string>
typedef unsigned char byte;
typedef std::string String;
#endif


// Serializer

struct Serializer
{
	byte* buffer;

	Serializer(byte buffer[]) : buffer(buffer){}

	template<typename T> Serializer& operator<<(const T& object)
	{
		write(object);
		return *this;
	}

	template<typename T, unsigned int N> Serializer& operator<<(const T(&array)[N])
	{
		write(array, N);
		return *this;
	}

	template<typename T> void write(const T& object)
	{
		*(T*)(buffer) = object;
		buffer += sizeof(T);
	}

	template<typename T> void write(const T* array, unsigned int length)
	{
		for (unsigned int i = 0; i < length; i++)
			write(array[i]);
	}

	void write(const char* string)
	{
		do
			write(*string);
		while (*(string++) != '\0');
	}
};

template<> inline void Serializer::write<String>(const String& string)
{
	write(string.c_str());
}


// Deserializer

struct Deserializer
{
	byte* buffer;

	Deserializer(byte buffer[]) : buffer(buffer){}

	template<typename T> Deserializer& operator>>(T& object)
	{
		object = read<T>();
		return *this;
	}

	template<typename T, unsigned int N> Deserializer& operator>>(T(&array)[N])
	{
		read(array, N);
		return *this;
	}

	Deserializer& operator>>(char* string)
	{
		read(string);
		return *this;
	}

	template<typename T> T read()
	{
		byte* address = buffer;
		buffer += sizeof(T);
		return *(T*)(address);
	}

	template<typename T> void read(T* array, unsigned int length)
	{
		for (unsigned int i = 0; i < length; i++)
			array[i] = read<T>();
	}

	void read(char* string)
	{
		do
			*string = read<char>();
		while ((*string++) != '\0');
	}
};

template<> inline String Deserializer::read<String>()
{
	String string((char*)(buffer));
	buffer += string.length() + 1;
	return string; 
}

#endif // __SERIALUTILS_H__
