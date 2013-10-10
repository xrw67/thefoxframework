#ifndef _THEFOX_LOGSTREAM_H_
#define _THEFOX_LOGSTREAM_H_

#include <base/noncopyable.h>

namespace thefox
{

template<int SIZE>
class FixBuffer : noncopyable
{
public:
	FixBuffer()
		:m_curPtr(m_data)
	{}
	~FixBuffer(){}
	void append(const char *buf, size_t len)
	{
		if (static_cast<size_t>(Avail()) > len)
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
private:
	char _data[SIZE];
	char *_curPtr;
};


class LogStream : noncopyable
{
	typedef LogStream self;
	typedef FixedBuffer Buffer;

public:
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
		m_buffer.Append(v ? "1" : "0", 1);
		return *this;
	}
	
	self &operator<<(float v)
	{
		*this << static_cast<double>(v);
		return *this;
	}
	self &operator<<(double);
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
	
	void append(const char *data, int len) { _buffer.append(data, len); }
	const Buffer &buffer() const { return _buffer; }
	const resetBuffer() { _buffer.Reset(); }

private:
	template<typename T>
	void FormatInteger(T v);
	Buffer _buffer;

	static const int kMaxNumericSize = 32;
};

}

#endif // _THEFOX_LOGSTREAM_H_