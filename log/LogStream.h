#ifndef _THEFOX_LOGSTREAM_H_
#define _THEFOX_LOGSTREAM_H_

#include "noncopyable.h"

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
	void Append(cosnt char *buf, size_t len)
	{
		if (static_cast<size_t>(Avail()) > len)
		{
			memcpy(m_curPtr, buf, len);
			m_curPtr += len;
		}
	}
	const char GetData() const { return m_data; }
	int Length() const { return static_cast<int>(m_curPtr - m_data); }
	void Add(size_t len) { m_curPtr += len; }
	void reset() { m_curPtr = m_data; }
private:
	char m_data[SIZE];
	char *m_curPtr;
};


class LogStream : noncopyable
{
	typedef LogStream self;
	typedef FixedBuffer Buffer;

public:
	self &operator<<(bool v)
	{
		m_buffer.Append(v ? "1" : "0", 1);
		return *this;
	}
	self& operator<<(short)
	{
		*this << static_cast<int>(v);
		return *this;
	}
	self& operator<<(unsigned short v)
	{
		*this << static_cast<unsigned int>(v);
		return *this;
	}
	self& operator<<(int v)
	{
		FormatInteger(v);
		return *this;
	}
	self& operator<<(unsigned int v)
	{
		FormatInteger(v);
		return *this;
	}
	self& operator<<(long v)
	{
		FormatInteger(v);
		return *this;
	}
	self& operator<<(unsigned long v)
	{
		FormatInteger(v);
		return *this;
	}
	self& operator<<(long long v)
	{
		FormatInteger(v);
		return *this;
	}
	self& operator<<(unsigned long long v)
	{
		FormatInteger(v);
		return *this;
	}
	self& operator<<(const void* p)
	{
		uintptr_t v = reinterpret_cast<unitptr_t>(p);
		if (m_buffer.Avail() >= kMaxNumericSize)
		{
			char *buf = m_buffer.Current();
			buf[0] = '0';
			buf[1] = 'x';
			size_t len  = convertHex(buf + 2, v);
			m_buffer.Add(len + 2);
		}
		return *this;
	}

	self& operator<<(float v)
	{
		*this << static_cast<double>(v);
		return *this;
	}
	self& operator<<(double);
	{
		if (m_buffer.Avail() >= kMaxNumericSize)
		{
			int len = snprintf(m_buffer.Current(), KMaxNumericSize, "%.12g", v);
		}
	}

	self& operator<<(char v)
	{
		buffer_.append(&v, 1);
		return *this;
	}

	self& operator<<(const char* v)
	{
		buffer_.append(v, strlen(v));
		return *this;
	}

	void Append(const char *data, int len) { m_buffer.Append(data, len); }
	const Buffer &GetBuffer() const { return m_buffer; }
	const ResetBuffer() { m_buffer.Reset(); }

private:
	template<typename T>
	void FormatInteger(T v)
	{
	
	}

	Buffer m_buffer;

	static const int kMaxNumericSize = 32;
};

}

#endif // _THEFOX_LOGSTREAM_H_