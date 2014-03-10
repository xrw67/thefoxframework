#ifndef _THEFOX_LOGSTREAM_H_
#define _THEFOX_LOGSTREAM_H_

#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include <base/Types.h>
#include <base/noncopyable.h>

namespace thefox
{

const int kBufferSize = 4096;

template<int SIZE>
class FixedBuffer : noncopyable
{
public:
	FixedBuffer()
		:_curPtr(_data)
	{}
	~FixedBuffer(){}
	void append(const char *buf, size_t len)
	{
		if (static_cast<size_t>(avail()) > len)
		{
			memcpy(_curPtr, buf, len);
			_curPtr += len;
		}
	}
	char *current() {return _curPtr; }
	const char *data() const { return _data; }
	int length() const { return static_cast<int>(_curPtr - _data); }
	void add(size_t len) { _curPtr += len; }
	void reset() { _curPtr = _data; }
	int avail() { return static_cast<int>(end() - _curPtr); }
private:
	const char* end() const { return _data + sizeof(_data); }
	char _data[SIZE];
	char *_curPtr;
};


class LogStream : noncopyable
{
public:
	typedef LogStream self;
	typedef FixedBuffer<kBufferSize> Buffer;

	self &operator<<(short);
	self &operator<<(unsigned short v);
	self &operator<<(int v);
	self &operator<<(unsigned int v);
	self &operator<<(long v);
	self &operator<<(unsigned long v);
	self &operator<<(long long v);
	self &operator<<(unsigned long long v);
	self &operator<<(const void* p);

	self &operator<<(bool v)
	{
		_buffer.append(v ? "1" : "0", 1);
		return *this;
	}
	
	self &operator<<(float v)
	{
		*this << static_cast<double>(v);
		return *this;
	}
	self &operator<<(double v)
	{
		if (_buffer.avail() >= kMaxNumericSize)
		{
			int len = snprintf(_buffer.current(), kMaxNumericSize, "%.12g", v);
			_buffer.add(len);
		}
		return *this;
	}

	self &operator<<(char v)
	{
		_buffer.append(&v, 1);
		return *this;
	}

	self &operator<<(const char* v)
	{
		_buffer.append(v, strlen(v));
		return *this;
	}
	self &operator<<(const String& v)
	{
		_buffer.append(v.c_str(), v.size());
		return *this;
	}
	void append(const char *data, int len) { _buffer.append(data, len); }
	const Buffer &buffer() const { return _buffer; }
	const void resetBuffer() { _buffer.reset(); }

private:
	template<typename T>
	void formatInteger(T v);
	Buffer _buffer;

	static const int kMaxNumericSize = 32;
};

}

#endif // _THEFOX_LOGSTREAM_H_