#ifndef _THEFOX_STRING_UTIL_H_
#define _THEFOX_STRING_UTIL_H_

#include <base/Types.h>

namespace thefox
{

static const int kMaxStringLen = 100 * 1024;
inline int stoi(const string &str) { return (0 == str.length()) ? 0 : atoi(str.c_str()); }
inline unsigned int stoui(const string &str) { return (0 == str.length()) ? 0 : static_cast<unsigned int>(atoi(str.c_str()));} 
inline long stol(const string &str) { return (0 == str.length()) ? 0L : atol(str.c_str()); }
inline float stof(const string &str){ return (0 == str.length()) ? 0.0f : static_cast<float>(atof(str.c_str())); }
inline double stod(const string &str) { return (0 == str.length()) ? 0.0 : atof(str.c_str()); }
inline bool stob(const string &str) { return (0 == str.length() || str == "0" || str == "false" || str == "FALSE") ? false : true; }

inline string to_string(int val)
{
	char buf[32] = {0};
	sprintf(buf, "%d", val);
	return buf;
}

inline string &sformat(string &str, const char *format, ...)
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

stoupper
}

#endif // _THEFOX_STRING_UTIL_H_