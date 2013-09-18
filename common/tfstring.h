#include "datatype.h"

#define kMaxStringLen (1024*100)

class TFString
{
	THString() :m_string() {}
	THString(const char c) : m_string(c) {}
	THString(const std::string &s) : m_string(s) {}
	THString(const char *s) : m_string(s) {}
	THString(const CString *s) : 
	{
		m_string = s.GetBuffer();
		s.ReleaseBuffer();
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
		m_string.clear();
	}
	const int ToInt()
	{
		if (0 == GetLength())
		{
			return 0;
		}
		return atoi(m_string.c_str());
	}
	const unsigned int ToUInt()
	{
		if (0 == GetLength())
		{
			return 0;
		}
		return static_cast<unsigned int>(atoi(m_string.c_str()));
	}
	const long ToLong()
	{
		if (0 == GetLength())
		{
			return 0L;
		}
		return atol(m_string.c_str());
	}
	const float ToFloat()
	{
		if (0 == GetLength())
		{
			return 0.0f;
		}
		return static_cast<float>(atof(m_strnig.c_str()));
	}
	const double ToDouble()
	{
		if (0 == GetLength())
		{
			return 0.0;
		}
		return atof(m_strnig.c_str());
	}
	const ToBool()
	{
		if (0 == GetLength())
		{
			return false;
		}
		if (m_string == "0" || m_string == "false" || m_string == "FALSE")
		{
			return false;
		}
		return true;
	}
	const size_t GetLength()
	{
		return m_string.length();
	}
	const bool IsEmpty()
	{
		return m_string.empty();
	}
	
	bool Format(const char *foemat, ...)
	{
		bool bRet = false;
		m_string.clear();
		
		va_list ap;
		va_start(ap, format);
		
		char *buf = (char *)malloc(kMaxStringLen);
		if (buf)
		{
			vsnprintf(buf, kMaxStringLen, format, ap);
			m_string = buf;
			free(buf);
			bRet = true;
		}
		
		va_end(ap);
		return bRet;
	}
	void Clear()
	{
		m_string.Clear();
	}
	TFString SubString(int beginIndex, int len = -1)
	{
		TFString str;
		if (-1 == len)
		{
			str.m_string.substr(beginIndex);
		}
		else
		{
			str.m_string.substr(beginIndex, len);
		}
		return str;
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
	const char operator[](const int index)
	{
		return m_string[index];
	}
	TFString &operator=(char c)
	{
		m_string = c;
		return *this;
	}
	TFString &operator=(const char *s)
	{
		m_string  = s;
		return *this;
	}
	TFString &operator=(const std::string &s)
	{
		m_string  = s;
		return *this;
	}
	TFString &operator=(const CString &s)
	{
		m_string = s.GetBuffer();
		s.ReleaseBuffer();
		return *this;
	}
	TFString &operator=(const TFString &s)
	{
		m_string = s.m_string;
		return *this;
	}
	TFString operator+(const TFString &s1, const TFString &s2)
	{
		TFString str;
		str = s1.m_string + s2.m_string;
		return str;
		
	}
	TFString operator+(const TFString &s1, char c)
	{
		TFString str;
		str = s1.m_string + c;
		return str;
	}
	TFString operator+(char c, const TFString &s2)
	{
		TFString str;
		str = c + s2.m_string;
		return str;
	}
	TFString operator+(const TFString &s1, const char *s2)
	{
		TFString str;
		str = s1.m_string + s2;
		return str;
	}
	TFString operator+(const char *s1, const TFString &s2)
	{
		TFString str;
		str = s1 + s2.m_string;
		return str;
	}
	TFString operator+(const TFString &s1, const std::string &s2)
	{
		TFString str;
		str = s1.m_string + s2;
		return str;
	}
	TFString operator+(const std::string &s1, const TFString &s2)
	{
		TFString str;
		str = s1 + s2.m_string;
		return str;
	}
	TFString operator+(const TFString &s1, const CString &s2)
	{
		TFString str;
		str = s1.m_string + s2.GetBuffer();
		s2.ReleaseBuffer();
		return str;
	}
	TFString operator+(const CString &s1, const TFString &s2)
	{
		TFString str;
		str = s1.GetBuffer() + s2.m_string;
		s1.ReleaseBuffer();
		return str;
	}
	TFString &operator+=(char c)
	{
		m_string += c;
		return *this;
	}
	TFString &operator+=(const char *s)
	{
		m_string  += s;
		return *this;
	}
	TFString &operator+=(const std::string &s)
	{
		m_string  += s;
		return *this;
	}
	TFString &operator+=(const CString &s)
	{
		m_string += s.GetBuffer();
		s.ReleaseBuffer();
		return *this;
	}
	TFString &operator+=(const TFString &s)
	{
		m_string += s.m_string;
		return *this;
	}
	bool operator==(const TFString &s1, const TFString &s2)
	{
		return s1.m_string == s2.m_string;
	}
	bool operator==(const TFString &s1, const char *s2)
	{
		return s1.m_string == s2;
	}
	bool operator==(const char *s1, const TFString &s2)
	{
		return s1 == s2.m_string;
	}
	bool operator==(const TFString &s1, const std::string &s2)
	{
		return s1.m_string == s2;
	}
	bool operator==(const std::string &s1, const TFString &s2)
	{
		return s1 == s2.m_string;
	}
	bool operator==(const TFString &s1, const CString &s2)
	{
		return s2 == s1.m_string.c_str();
	}
	bool operator==(const CString &s1, const TFString &s2)
	{
		return s1 == s2.m_string.c_str();
	}
	bool operator!=(const TFString &s1, const TFString &s2) { return !(s1 == s2); }
	bool operator!=(const TFString &s1, const char *s2) { return !(s1 == s2); }
	bool operator!=(const char *s1, const TFString &s2) { return !(s1 == s2); }
	bool operator!=(const TFString &s1, const std::string &s2) { return !(s1 == s2); }
	bool operator!=(const std::string &s1, const TFString &s2) { return !(s1 == s2); }
	bool operator!=(const TFString &s1, const CString &s2) { return !(s1 == s2); }
	bool operator!=(const CString &s1, const TFString &s2) { return !(s1 == s2); }
public:
	static TFString *Create(const char *s)
	{
		TFString *pStr = new TFString(s);
		return pStr;
	}
	static TFString *Create(const std::string &s)
	{
		TFString *pStr = new TFString(s);
		return pStr;
	}
	
private:
	std::string m_string;
};