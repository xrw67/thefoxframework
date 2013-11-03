#ifndef _THEFOX_STRING_UTIL_H_
#define _THEFOX_STRING_UTIL_H_

#include <stdarg.h>
#include <base/Types.h>

#pragma warning(disable:4996)

namespace thefox
{

static const int kMaxStringLen = 100 * 1024;
inline int stoi(const String &str) { return (0 == str.length()) ? 0 : atoi(str.c_str()); }
inline unsigned int stoui(const String &str) { return (0 == str.length()) ? 0 : static_cast<unsigned int>(atoi(str.c_str()));} 
inline long stol(const String &str) { return (0 == str.length()) ? 0L : atol(str.c_str()); }
inline float stof(const String &str){ return (0 == str.length()) ? 0.0f : static_cast<float>(atof(str.c_str())); }
inline double stod(const String &str) { return (0 == str.length()) ? 0.0 : atof(str.c_str()); }
inline bool stob(const String &str) { return (0 == str.length() || str == "0" || str == "false" || str == "FALSE") ? false : true; }

inline String toString(int val)
{
	char buf[32] = {0};
	_snprintf(buf, sizeof(buf), "%d", val);
	return buf;
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

}
inline String trimRight(const String &str)
{

}

inline String trim(const String &str)
{

}

inline String toLower(const String &str)
{

}

inline String toUpper(const String &str)
{

}

inline String replace(const String &str, const String &from, const String &to, int start = 0)
{

}

inline void swap(String &ls, String &rs)
{
	String temp(ls);
	ls = rs;
	rs = temp;
}

}

#endif // _THEFOX_STRING_UTIL_H_