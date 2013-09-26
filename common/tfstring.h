/**
* @file tfstring.h
* @brief 字符串类
* @autuor macwe@qq.com
*/
#ifndef _TF_STRING_H_
#define _TF_STRING_H_

#pragma warning(disable:4996)

#include <stdio.h>
#include <stdarg.h>
#include <string>

#ifdef USE_NAMESPACE
namespace TheFox {
#endif

#ifdef __cpluscpus
extern "C" {
#endif

#define kMaxStringLen (1024*100)

class TFString
{
public:
	TFString() :m_string(){}
	TFString(const char c) : m_string(1, c){}
	TFString(const char *s) : m_string(s){}
	TFString(const std::string &s) : m_string(s){}
	TFString(const TFString &s) :m_string(s.m_string){}
	virtual ~TFString()
	{
		m_string.clear();
	}
	
	/// @brief convert string to int
	/// @return a int value
	const int ToInt() const
	{
		return (0 == Length()) ? 0 : atoi(m_string.c_str());
	}
	
	/// @brief convert string to unsigned int
	/// @return a unsigned int value
	const unsigned int ToUInt() const
	{
		return (0 == Length()) ? 0 : static_cast<unsigned int>(atoi(m_string.c_str()));
	}
	
	/// @brief convert string to long
	/// @return a log value
	const long ToLong() const
	{
		return (0 == Length()) ? 0L : atol(m_string.c_str());
	}
	
	/// @brief convert string to float
	/// @return a float value
	const float ToFloat() const
	{
		return (0 == Length()) ? 0.0f : static_cast<float>(atof(m_string.c_str()));
	}
	
	/// @brief convert string to double
	/// @return a double value
	const double ToDouble() const
	{
		return (0 == Length()) ? 0.0 : atof(m_string.c_str());
	}
	
	/// @brief convert string to bool
	/// @return a bool value
	const bool ToBool() const
	{
		if (0 == Length())
		{
			return false;
		}
		if (m_string == "0" || m_string == "false" || m_string == "FALSE")
		{
			return false;
		}
		return true;
	}
	
	/// @brief return length of string
	/// @return length of string
	const int Length() const
	{
		return m_string.length();
	}
	
	/// @brief return string is empty;
	/// @return true is string is empty, otherwise return false
	const bool IsEmpty() const
	{
		return m_string.empty();
	}
	
	/// @brief Get string with format like "printf"
	/// @return true if format success, otherwise return false 
	bool Format(const char *format, ...)
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
	
	/// @brief clear string
	/// @return void
	void Clear()
	{
		m_string.clear();
	}
	
	/// @brief Get substring 
	/// @param beginIndex [in] first char index 
	/// @param len[i] 
	/// @return a substring
	TFString SubString(int beginIndex, int len = -1) const
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
	/// @brief convert string to lower case
	/// @return this string
	TFString &ToLower()
	{
		for (int i = 0; i < Length(); i++)
		{
			if (m_string[i] >= 'A' && m_string[i] <= 'Z')
			{
				m_string[i] += 0x20;
			}
		}
	}
	/// @brief convert string to upper case
	/// @return this string
	TFString &ToUpper()
	{
		for (int i = 0; i < Length(); i++)
		{
			if (m_string[i] >= 'a' && m_string[i] <= 'z')
			{
				m_string[i] -= 0x20;
			}
		}
	}
	TFString &Trim()
	{
		TrimRight();
		TrimLeft();
		return *this;
	}
	TFString &TrimLeft()
	{
		std::string::size_type index = m_string.find_first_not_of(" \n\r\t");
		if (index != std::string::npos)
		{
			m_string = m_string.substr(index);
		}
		return *this;
	}
	TFString &TrimRight()
	{
		std::string::size_type index = m_string.find_last_not_of(" \n\r\t");
		if (index != std::string::npos)
		{
			m_string = m_string.substr(0, index + 1);
		}
		return *this;
	}
	const char At(int index)
	{
		return m_string.at(index);
	}
	const char *CStr()
	{
		return m_string.c_str();
	}
	int Compare(TFString &s)
	{
		return m_string.compare(s.m_string);
	}
	bool EqualsIgnoreCase(TFString &s)
	{
		return this->ToLower() == s.ToLower();
	}
	TFString &Replace(const char oldChar, const char newChar)
	{
		for (int i = 0; i < Length(); ++i)
		{
			if (oldChar == m_string[i])
			{
				m_string[i] = newChar;
			}
		}
		return *this;
	}
	TFString &Replace(const char *oldStr, const char *newStr)
	{
		int oldStrLen = strlen(oldStr);
		int newStrLen = strlen(newStr);
		int pos = 0;
		while ((pos = IndexOf(oldStr, pos)) != -1)
		{
			m_string.replace(pos, oldStrLen, newStr);
			pos += newStrLen;
		}

		return *this;
	}
	const int IndexOf(const char c, const int pos = 0)
	{
		return static_cast<int>(m_string.find(c, pos));
	}
	const int IndexOf(const char *s, const int pos = 0)
	{
		return static_cast<int>(m_string.find(s, pos));
	}
	const int IndexOf(const TFString &s,const int pos = 0)
	{
		return static_cast<int>(m_string.find(s.m_string, pos));
	}
	const int LastIndexOf(const char c, const int pos = -1)
	{
		return static_cast<int>(m_string.rfind(c, pos));
	}
	const int LastIndexOf(const char *s, const int pos = -1)
	{
		return static_cast<int>(m_string.rfind(s, pos));
	}
	const int LastIndexOf(const TFString &s,const int pos = -1)
	{
		return static_cast<int>(m_string.rfind(s.m_string, pos));
	}
	const TFString &Insert(const char c, const int pos)
	{
		m_string.insert(pos, 1, c);
		return *this;
	}
	const TFString &Insert(const char *s, const int pos)
	{
		m_string.insert(pos, s);
		return *this;
	}
	const TFString &Insert(const TFString &s, const int pos)
	{
		m_string.insert(pos, s.m_string);
		return *this;
	}

	const TFString &Reverse()
	{
		size_t kLength = Length();
		char tmpChar = '\0';
		
		for (size_t i = 0; i < (kLength / 2); ++i)
		{
			tmpChar = m_string[i];
			m_string[i] = m_string[kLength - i - 1];
			m_string[kLength - i - 1] = tmpChar;
		}
		return *this;
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
	TFString &operator=(const TFString &s)
	{
		m_string = s.m_string;
		return *this;
	}

	friend TFString operator+(const TFString &s1, const TFString &s2);
	friend TFString operator+(const TFString &s1, char c);
	friend TFString operator+(char c, const TFString &s2);
	friend TFString operator+(const TFString &s1, const char *s2);
	friend TFString operator+(const char *s1, const TFString &s2);
	friend TFString operator+(const TFString &s1, const std::string &s2);
	friend TFString operator+(const std::string &s1, const TFString &s2);

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
	TFString &operator+=(const TFString &s)
	{
		m_string += s.m_string;
		return *this;
	}
	friend bool operator==(const TFString &s1, const TFString &s2);
	friend bool operator==(const TFString &s1, const char *s2);
	friend bool operator==(const char *s1, const TFString &s2);
	friend bool operator==(const TFString &s1, const std::string &s2);
	friend bool operator==(const std::string &s1, const TFString &s2);

	friend bool operator!=(const TFString &s1, const TFString &s2);
	friend bool operator!=(const TFString &s1, const char *s2);
	friend bool operator!=(const char *s1, const TFString &s2);
	friend bool operator!=(const TFString &s1, const std::string &s2);
	friend bool operator!=(const std::string &s1, const TFString &s2);

public:
	/// Create a string with value type with c string
	static TFString Create(const char *s)
	{
		TFString str(s);
		return str;
	}
	
	/// Create a string with value type with std::string
	static TFString Create(const std::string &s)
	{
		TFString str(s);
		return str;
	}
	
	/// Create a string with value type with int
	static TFString Create(const int i)
	{
		TFString str;
		char buf[64] = {0};
		str.m_string = _itoa(i, buf, 10);
		return str;
	}
	
	/// Create a string with value type with unsigned int
	static TFString Create(const unsigned int ui)
	{
		TFString str;
		char buf[64] = {0};
		_snprintf(buf, sizeof(buf), "%u", ui);
		str.m_string = buf;
		return str;
	}
	/// Create a string with value type with long
	static TFString Create(const long l)
	{
		TFString str;
		char buf[64] = {0};
		str.m_string = _ltoa(l, buf, 10);
		return str;
	}
	
	/// Create a string with value type with float
	static TFString Create(const float f)
	{
		TFString str;
		char buf[64] = {0};
		_snprintf(buf, sizeof(buf), "%f", f);
		str.m_string = buf;
		return str;
	}
	
	/// Create a string with value type with double
	static TFString Create(const double d)
	{
		TFString str;
		char buf[64] = {0};
		_snprintf(buf, sizeof(buf), "%f", d);
		str.m_string = buf;
		return str;
	}

	static TFString CreateWithFormat(const char *format, ...)
	{
		TFString str;
		va_list ap;
		va_start(ap, format);
		
		char *buf = (char *)malloc(kMaxStringLen);
		if (buf)
		{
			vsnprintf(buf, kMaxStringLen, format, ap);
			str = buf;
			free(buf);
		}
		
		va_end(ap);
		return str;
	}
private:
	std::string m_string;
};


inline TFString operator+(const TFString &s1, const TFString &s2)
{
	TFString str;
	str = s1.m_string + s2.m_string;
	return str;
		
}
inline TFString operator+(const TFString &s1, char c)
{
	TFString str;
	str = s1.m_string + c;
	return str;
}
inline TFString operator+(char c, const TFString &s2)
{
	TFString str;
	str = c + s2.m_string;
	return str;
}
inline TFString operator+(const TFString &s1, const char *s2)
{
	TFString str;
	str = s1.m_string + s2;
	return str;
}
inline TFString operator+(const char *s1, const TFString &s2)
{
	TFString str;
	str = s1 + s2.m_string;
	return str;
}
inline TFString operator+(const TFString &s1, const std::string &s2)
{
	TFString str;
	str = s1.m_string + s2;
	return str;
}
inline TFString operator+(const std::string &s1, const TFString &s2)
{
	TFString str;
	str = s1 + s2.m_string;
	return str;
}
inline bool operator==(const TFString &s1, const TFString &s2)
{
	return s1.m_string == s2.m_string;
}
inline bool operator==(const TFString &s1, const char *s2)
{
	return s1.m_string == s2;
}
inline bool operator==(const char *s1, const TFString &s2)
{
	return s1 == s2.m_string;
}
inline bool operator==(const TFString &s1, const std::string &s2)
{
	return s1.m_string == s2;
}
inline bool operator==(const std::string &s1, const TFString &s2)
{
	return s1 == s2.m_string;
}
inline bool operator!=(const TFString &s1, const TFString &s2)
{ 
	return !(s1 == s2); 
}
inline bool operator!=(const TFString &s1, const char *s2)
{ 
	return !(s1 == s2);
}
inline bool operator!=(const char *s1, const TFString &s2) 
{ 
	return !(s1 == s2);
}
inline bool operator!=(const TFString &s1, const std::string &s2) 
{
	return !(s1 == s2); 
}
inline bool operator!=(const std::string &s1, const TFString &s2) 
{ 
	return !(s1 == s2); 
}

#ifdef __cpluscpus
};
#endif

#ifdef USE_NAMESPACE
};
#endif

#endif // _TF_STRING_H_