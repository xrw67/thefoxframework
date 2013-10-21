#include <algorithm>
#include "LogStream.h"

namespace thefox
{
// Efficient Integer to String Conversions, by Matthew Wilson.
const char digits[] = "9876543210123456789";
const char* zero = digits + 9;
const char digitsHex[] = "0123456789ABCDEF";

template<typename T>
inline size_t convert(char buf[], T value)
{
	T i = value;
	char* p = buf;

	do
	{
		int lsd = static_cast<int>(i % 10);
		i /= 10;
		*p++ = zero[lsd];
	} while (i != 0);

	if (value < 0)
	{
		*p++ = '-';
	}
	*p = '\0';
	std::reverse(buf, p);

	return p - buf;
}

inline size_t convertHex(char buf[], uintptr_t value)
{
	uintptr_t i = value;
	char* p = buf;

	do
	{
		int lsd = i % 16;
		i /= 16;
		*p++ = digitsHex[lsd];
	} while (i != 0);

	*p = '\0';
	std::reverse(buf, p);

	return p - buf;
}

}
using namespace thefox;

template<typename T>
void LogStream::formatInteger(T v)
{
	if (_buffer.avail() >= kMaxNumericSize)
	{
		size_t len = convert(_buffer.current(), v);
		_buffer.add(len);
	}
}

LogStream &LogStream::operator<<(short v)
{
	*this << static_cast<int>(v);
	return *this;
}
LogStream &LogStream::operator<<(unsigned short v)
{
	*this << static_cast<unsigned int>(v);
	return *this;
}
LogStream &LogStream::operator<<(int v)
{
	formatInteger(v);
	return *this;
}
LogStream &LogStream::operator<<(unsigned int v)
{
	formatInteger(v);
	return *this;
}
LogStream &LogStream::operator<<(long v)
{
	formatInteger(v);
	return *this;
}
LogStream &LogStream::operator<<(unsigned long v)
{
	formatInteger(v);
	return *this;
}
LogStream &LogStream::operator<<(long long v)
{
	formatInteger(v);
	return *this;
}
LogStream &LogStream::operator<<(unsigned long long v)
{
	formatInteger(v);
	return *this;
}

LogStream &LogStream::operator<<(const void* p)
{
	uintptr_t v = reinterpret_cast<uintptr_t>(p);
	if (_buffer.avail() >= kMaxNumericSize)
	{
		char *buf = _buffer.current();
		buf[0] = '0';
		buf[1] = 'x';
		size_t len  = convertHex(buf + 2, v);
		_buffer.add(len + 2);
	}
	return *this;
}