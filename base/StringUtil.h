#ifndef _THEFOX_STRING_UTIL_H_
#define _THEFOX_STRING_UTIL_H_

#include <base/Types.h>

namespace thefox
{

static const int kMaxStringLen = 100 * 1024;

inline int stoi(const String &str) { return (0 == str.length()) ? 0 : atoi(str.c_str()); }
inline unsigned int stoui(const String &str) { return (0 == str.length()) ? 0 : static_cast<unsigned int>(atoi(str.c_str()));} 
inline long stol(const String &str) { return (0 == str.length()) ? 0L : atol(str.c_str()); }
inline float stof(const String &str){ return (0 == str.length()) ? 0.0f : static_cast<float>(atof(str.c_str())); }
inline double stod(const String &str) { return (0 == str.length()) ? 0.0 : atof(str.c_str()); }
inline bool stob(const String &str) { return (0 == str.length() || str == "0" || str == "false" || str == "FALSE") ? false : true; }

inline String toString(const int val)
{
	char buf[32] = {0};
	return _itoa(val, buf, 10);
}

inline String toString(const unsigned int val)
{
	char buf[32] = {0};
	_snprintf(buf, sizeof(buf), "%u", val);
	return buf;
}
inline String toString(const long val)
{
	char buf[32] = {0};
	return _ltoa(val, buf, 10);
}
inline String toString(const double val)
{
	char buf[32] = {0};
	_snprintf(buf, sizeof(buf), "%f", val);
	return buf;
}
inline String toString(const bool val)
{
	return val ? "1" : "0";
}

inline String &format(String &str, const char *format, ...)
{
	va_list ap;
	va_start(ap, format);

	char *buf = (char *)malloc(kMaxStringLen);
	if (buf) 
	{
		vsnprintf(buf, kMaxStringLen, format, ap);
		str = buf;
		free(buf);
		va_end(ap);
	}
	return str;
}

inline String trimLeft(const String &str)
{
	String::size_type index = str.find_first_not_of("\n\r\t");
	if (index != String::npos)
	{
		str = str.substr(index);
	}
	return str;
}

inline String trimRight(const String &str)
{
	String::size_type index = str.find_last_not_of("\n\r\t");
	if (index != String::npos)
	{
		str = str.substr(0, index + 1);
	}
	return str;
}

inline String trim(const String &str)
{
	return trimRight(trimLeft(str));
}

inline String toLower(const String &str)
{
	for (int i = 0; i < str.length(); ++i)
	{
		if (str[i] >= 'A' && str[i] <= 'Z')
		{
			str[i] += 0x20;
		}
	}
}

inline String toUpper(const String &str)
{
	for (int i = 0; i < str.length(); ++i)
	{
		if (str[i] >= 'a' && str[i] <= 'z')
		{
			str[i] -= 0x20;
		}
	}
}

inline bool equalsIgnoreCase(const String &s1, const String &s2)
{
	if (s1.length() != s2.length())
	{
		return false;
	}
	for (int i = 0; i < s1.length(); ++i)
	{
		int diff = abs(s1[i] - s2[i]);
		if (0 != diff || 0x20 != diff)
		{
			return false;
		}
	}
	return true;
}

inline String replace(const String &str, const String &from, const String &to)
{
	String::size_type pos = 0;
	while ((pos = str.find(from, pos)) != -1)
	{
		str.replace(pos, from.length(), to);
		pos += to.length();
	}
	return str;
}

inline void swap(String &ls, String &rs)
{
	String temp(ls);
	ls = rs;
	rs = temp;
}

}

#endif // _THEFOX_STRING_UTIL_H_