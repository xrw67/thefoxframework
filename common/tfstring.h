#include "datatype.h"

class TFString
{
	THString()
	{
	}
	THString(const std::string &s)
	{
		m_string = s;
	}
	THString(const char *s)
	{
		m_string = s;
	}
	THString(const CString *s)
	{
		m_string = s.GetBuffer();
		s.ReleaseBuffer();
	}
	THString(const char c)
	{
		m_string = c;
	}
	THString(const int i)
	{
		char buf[64] = {0};
		m_string = _itoa(i, buf, 10);
	}
	THString(const unsigned int ui)
	{
		char buf[64] = {0};
		_snprintf(buf, sizeof(buf), "%u", ui);
		m_string = buf;
	}
	THString(const long l)
	{
		char buf[64] = {0};
		m_string = _ltoa(l, buf, 10);
	}
	THString(const float f)
	{
		char buf[64] = {0};
		_snprintf(buf, sizeof(buf), "%f", f);
		m_string = buf;
	}
	THString(const double d)
	{
		char buf[64] = {0};
		_snprintf(buf, sizeof(buf), "%d", d);
		m_string = buf;
	}
	~THString()
	{
	}
	const int ToInt()
	{
		return atoi(m_string.c_str());
	}
	const unsigned int ToUInt()
	{
		unsigned int value = 0;
		sscanf(m_string.c_str(), "%u", &value);
		return value;
	}
	const long ToLong()
	{
		return atol(m_string.c_str());
	}
	const float ToFloat()
	{
		
	}
	const double ToDouble()
	{
		return atof(m_strnig.c_str());
	}
	
	const size_t GetLength()
	{
		return m_string.length();
	}
	const bool IsEmpty()
	{
		return m_string.empty();
	}
	
	void Format(const char *fmt, ...)
	{
	
	}
	TFString &Concat(TFString &s)
	{
	}
	TFString SubString(int beginIndex, int number = 0)
	{
	}
	TFString &ToLower()
	{
	}
	TFString &ToUpper()
	{
	}
	TFString &Trim()
	{
	}
	TFString &TrimLeft()
	{
	}
	TFString &TrimRight()
	{
	}
	
	TFString &operator=(char c)
	{
		m_string = c;
	}
	TFString &operator=(const char *s)
	{
		m_string  = s;
	}
	TFString &operator=(const std::string &s)
	{
		m_string  = s;
	}
	TFString &operator=(const CString &s)
	{
		m_string = s.GetBuffer();
		s.ReleaseBuffer();
	}
	TFString &operator=(const TFString &s)
	{
		m_string = s.m_string;
	}
	TFString operator+(const TFString &s1, const TFString &s2)
	{
	}
	TFString operator+(const TFString &s1, char c)
	{
	}
	TFString operator+(char c, const TFString &s2)
	{
	}
	TFString operator+(const TFString &s1, const char *s2)
	{
	}
	TFString operator+(const char *s1, const TFString &s2)
	{
	}
	TFString operator+(const TFString &s1, const std::string &s2)
	{
	}
	TFString operator+(const std::string &s1, const TFString &s2)
	{
	}
	TFString operator+(const TFString &s1, const CString &s2)
	{
	}
	TFString operator+(const CString &s1, const TFString &s2)
	{
	}
	TFString &operator+=(char c)
	{
		m_string += c;
	}
	TFString &operator+=(const char *s)
	{
		m_string  += s;
	}
	TFString &operator+=(const std::string &s)
	{
		m_string  += s;
	}
	TFString &operator+=(const CString &s)
	{
		m_string += s.GetBuffer();
		s.ReleaseBuffer();
	}
	TFString &operator+=(const TFString &s)
	{
		m_string += s.m_string;
	}
	bool operator==(const TFString &s1, const TFString &s2)
	{
	
	}
	bool operator==(const TFString &s1, const char *s2)
	{
	
	}
	bool operator==(const char *s1, const TFString &s2)
	{
	
	}
	bool operator==(const TFString &s1, const std::string &s2)
	{
	
	}
	bool operator==(const std::string &s1, const TFString &s2)
	{
	
	}
	bool operator==(const TFString &s1, const CString &s2)
	{
	
	}
	bool operator==(const CString &s1, const TFString &s2)
	{
	
	}
	bool operator!=(const TFString &s1, const TFString &s2)
	{
	
	}
	bool operator!=(const TFString &s1, const char *s2)
	{
	
	}
	bool operator!=(const char *s1, const TFString &s2)
	{
	
	}
	bool operator!=(const TFString &s1, const std::string &s2)
	{
	
	}
	bool operator!=(const std::string &s1, const TFString &s2)
	{
	
	}
	bool operator!=(const TFString &s1, const CString &s2)
	{
	
	}
	bool operator!=(const CString &s1, const TFString &s2)
	{
	
	}
private:
	std::string m_string;
};