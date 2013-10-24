/**
* @file String.h
* @brief 字符串类
* @author macwe@qq.com
*/
#ifndef _THEFOX_STRING_H_
#define _THEFOX_STRING_H_

#pragma warning(disable:4996)

#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <math.h>

namespace thefox 
{

class String
{
public:
	String() : _data() {}
	String(const char c) : _data(1, c) {}
	String(const char *s) : _data(s) {}
	String(const std::string &s) : _data(s) {}
	String(const String &s) : _data(s._data) {}
	virtual ~String() { _data.clear(); }
	
	/// @brief convert string to int
	/// @return a int value
	const int toInt() const { return (0 == length()) ? 0 : atoi(_data.c_str()); }
	/// @brief convert string to unsigned int
	/// @return a unsigned int value
	const unsigned int toUInt() const { return (0 == length()) ? 0 : static_cast<unsigned int>(atoi(_data.c_str())); }
	/// @brief convert string to long
	/// @return a log value
	const long toLong() const { return (0 == length()) ? 0L : atol(_data.c_str()); }
	/// @brief convert string to float
	/// @return a float value
	const float toFloat() const { return (0 == length()) ? 0.0f : static_cast<float>(atof(_data.c_str())); }
	/// @brief convert string to double
	/// @return a double value
	const double toDouble() const { return (0 == length()) ? 0.0 : atof(_data.c_str()); }
	/// @brief convert string to bool
	/// @return a bool value
	const bool toBool() const
	{
		if (0 == length())
		{
			return false;
		}
		if (_data == "0" || _data == "false" || _data == "FALSE")
		{
			return false;
		}
		return true;
	}
	
	/// @brief return length of string
	/// @return length of string
	const int length() const { return _data.length(); }
	
	/// @brief return string is empty;
	/// @return true is string is empty, otherwise return false
	const bool IsEmpty() const { return _data.empty(); }
	
	/// @brief Get string with format like "printf"
	/// @return true if format success, otherwise return false 
	bool format(const char *format, ...)
	{
		bool bRet = false;
		_data.clear();
		
		va_list ap;
		va_start(ap, format);
		
		char *buf = (char *)malloc(kMaxStringLen);
		if (buf)
		{
			vsnprintf(buf, kMaxStringLen, format, ap);
			_data = buf;
			free(buf);
			bRet = true;
		}
		
		va_end(ap);
		return bRet;
	}
	
	/// @brief clear string
	/// @return void
	void clear() { _data.clear(); }
	
	/// @brief Get substring 
	/// @param beginIndex [in] first char index 
	/// @param len[i] 
	/// @return a substring
	String subString(int beginIndex, int len = -1) const
	{
		String str;
		return (-1 == len) ? str._data.substr(beginIndex) : str._data.substr(beginIndex, len);
	}
	/// @brief convert string to lower case
	/// @return this string
	String &toLower()
	{
		for (int i = 0; i < length(); i++)
		{
			if (_data[i] >= 'A' && _data[i] <= 'Z')
			{
				_data[i] += 0x20;
			}
		}
	}
	/// @brief convert string to upper case
	/// @return this string
	String &toUpper()
	{
		for (int i = 0; i < length(); i++)
		{
			if (_data[i] >= 'a' && _data[i] <= 'z')
			{
				_data[i] -= 0x20;
			}
		}
	}
	String &trim()
	{
		trimRight();
		trimLeft();
		return *this;
	}
	String &trimLeft()
	{
		std::string::size_type index = _data.find_first_not_of(" \n\r\t");
		if (index != std::string::npos)
		{
			_data = _data.substr(index);
		}
		return *this;
	}
	String &trimRight()
	{
		std::string::size_type index = _data.find_last_not_of(" \n\r\t");
		if (index != std::string::npos)
		{
			_data = _data.substr(0, index + 1);
		}
		return *this;
	}
	const char at(int index) const { return _data.at(index); }
	const char *c_str() const { return _data.c_str(); }
	int compare(String &s) const { return _data.compare(s._data); }
	bool equalsIgnoreCase(String &s) 
	{	
		if (length() != s.length())
		{
			return false;
		}
		for (int i = 0; i < length(); ++i)
		{
			int diff = abs(at(i) - s.at(i));
			if (0 != diff || 0x20 != diff)
			{
				return false;
			}
		}
		return true;
	}
	
	String &replace(const char oldChar, const char newChar)
	{
		for (int i = 0; i < length(); ++i)
		{
			if (oldChar == _data[i])
			{
				_data[i] = newChar;
			}
		}
		return *this;
	}
	String &replace(const char *oldStr, const char *newStr)
	{
		int oldStrLen = strlen(oldStr);
		int newStrLen = strlen(newStr);
		int pos = 0;
		while ((pos = indexOf(oldStr, pos)) != -1)
		{
			_data.replace(pos, oldStrLen, newStr);
			pos += newStrLen;
		}

		return *this;
	}
	
	const int indexOf(const char c, const int pos = 0) const { return static_cast<int>(_data.find(c, pos)); }
	const int indexOf(const char *s, const int pos = 0) const { return static_cast<int>(_data.find(s, pos)); }
	const int indexOf(const String &s,const int pos = 0) const { return static_cast<int>(_data.find(s._data, pos)); }
	const int lastIndexOf(const char c, const int pos = -1) const { return static_cast<int>(_data.rfind(c, pos)); }
	const int lastIndexOf(const char *s, const int pos = -1) const { return static_cast<int>(_data.rfind(s, pos)); }
	const int lastIndexOf(const String &s,const int pos = -1) const { return static_cast<int>(_data.rfind(s._data, pos)); }
	
	const String &insert(const char c, const int pos)
	{
		_data.insert(pos, 1, c);
		return *this;
	}
	const String &insert(const char *s, const int pos)
	{
		_data.insert(pos, s);
		return *this;
	}
	const String &insert(const String &s, const int pos)
	{
		_data.insert(pos, s._data);
		return *this;
	}

	const String &reverse()
	{
		size_t kLength = length();
		char tmpChar = '\0';
		
		for (size_t i = 0; i < (kLength / 2); ++i)
		{
			tmpChar = _data[i];
			_data[i] = _data[kLength - i - 1];
			_data[kLength - i - 1] = tmpChar;
		}
		return *this;
	}

	const char operator[](const int index) const { return _data[index]; }
	
	String &operator=(char c)
	{
		_data = c;
		return *this;
	}
	String &operator=(const char *s)
	{
		_data  = s;
		return *this;
	}
	String &operator=(const std::string &s)
	{
		_data  = s;
		return *this;
	}
	String &operator=(const String &s)
	{
		_data = s._data;
		return *this;
	}

	String &operator+=(char c)
	{
		_data += c;
		return *this;
	}
	String &operator+=(const char *s)
	{
		_data  += s;
		return *this;
	}
	String &operator+=(const std::string &s)
	{
		_data  += s;
		return *this;
	}
	String &operator+=(const String &s)
	{
		_data += s._data;
		return *this;
	}

	friend String operator+(const String &s1, const String &s2);
	friend String operator+(const String &s1, char c);
	friend String operator+(char c, const String &s2);
	friend String operator+(const String &s1, const char *s2);
	friend String operator+(const char *s1, const String &s2);
	friend String operator+(const String &s1, const std::string &s2);
	friend String operator+(const std::string &s1, const String &s2);

	friend bool operator==(const String &s1, const String &s2);
	friend bool operator==(const String &s1, const char *s2);
	friend bool operator==(const char *s1, const String &s2);
	friend bool operator==(const String &s1, const std::string &s2);
	friend bool operator==(const std::string &s1, const String &s2);

	friend bool operator!=(const String &s1, const String &s2);
	friend bool operator!=(const String &s1, const char *s2);
	friend bool operator!=(const char *s1, const String &s2);
	friend bool operator!=(const String &s1, const std::string &s2);
	friend bool operator!=(const std::string &s1, const String &s2);

public:
	/// Create a string with value type with c string
	static String create(const char *s)
	{
		String str(s);
		return str;
	}
	
	/// Create a string with value type with std::string
	static String create(const std::string &s)
	{
		String str(s);
		return str;
	}
	
	/// Create a string with value type with int
	static String create(const int i)
	{
		String str;
		char buf[64] = {0};
		str._data = _itoa(i, buf, 10);
		return str;
	}
	
	/// Create a string with value type with unsigned int
	static String create(const unsigned int ui)
	{
		String str;
		char buf[64] = {0};
		_snprintf(buf, sizeof(buf), "%u", ui);
		str._data = buf;
		return str;
	}
	/// Create a string with value type with long
	static String create(const long l)
	{
		String str;
		char buf[64] = {0};
		str._data = _ltoa(l, buf, 10);
		return str;
	}
	
	/// Create a string with value type with float
	static String create(const float f)
	{
		String str;
		char buf[64] = {0};
		_snprintf(buf, sizeof(buf), "%f", f);
		str._data = buf;
		return str;
	}
	
	/// Create a string with value type with double
	static String create(const double d)
	{
		String str;
		char buf[64] = {0};
		_snprintf(buf, sizeof(buf), "%f", d);
		str._data = buf;
		return str;
	}

	static String createWithFormat(const char *format, ...)
	{
		String str;
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
	std::string _data;
	static const int kMaxStringLen = 1024*100;
};

inline String operator+(const String &s1, const String &s2)
{
	String str;
	str = s1._data + s2._data;
	return str;
		
}
inline String operator+(const String &s1, char c)
{
	String str;
	str = s1._data + c;
	return str;
}
inline String operator+(char c, const String &s2)
{
	String str;
	str = c + s2._data;
	return str;
}
inline String operator+(const String &s1, const char *s2)
{
	String str;
	str = s1._data + s2;
	return str;
}
inline String operator+(const char *s1, const String &s2)
{
	String str;
	str = s1 + s2._data;
	return str;
}
inline String operator+(const String &s1, const std::string &s2)
{
	String str;
	str = s1._data + s2;
	return str;
}
inline String operator+(const std::string &s1, const String &s2)
{
	String str;
	str = s1 + s2._data;
	return str;
}
inline bool operator==(const String &s1, const String &s2)
{
	return s1._data == s2._data;
}
inline bool operator==(const String &s1, const char *s2)
{
	return s1._data == s2;
}
inline bool operator==(const char *s1, const String &s2)
{
	return s1 == s2._data;
}
inline bool operator==(const String &s1, const std::string &s2)
{
	return s1._data == s2;
}
inline bool operator==(const std::string &s1, const String &s2)
{
	return s1 == s2._data;
}
inline bool operator!=(const String &s1, const String &s2)
{ 
	return !(s1 == s2); 
}
inline bool operator!=(const String &s1, const char *s2)
{ 
	return !(s1 == s2);
}
inline bool operator!=(const char *s1, const String &s2) 
{ 
	return !(s1 == s2);
}
inline bool operator!=(const String &s1, const std::string &s2) 
{
	return !(s1 == s2); 
}
inline bool operator!=(const std::string &s1, const String &s2) 
{ 
	return !(s1 == s2); 
}

} // namespace thefox

#endif // _THEFOX_STRING_H_
