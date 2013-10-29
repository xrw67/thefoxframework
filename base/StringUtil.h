#ifndeF _THEFOX_STRING_UTIL_H_
#define _THEFOX_STRING_UTIL_H_

#include <base/Types.h>

namespace thefox
{

inline int stoi (const std::string&  str, size_t* idx = 0, int base = 10)
{
	return atoi(str.c_str());
}

inline std::string to_string(int val)
{
	char buf[32] = {0};
	sprintf(buf, "%d", val);
	return buf;
}

}

#endif // _THEFOX_STRING_UTIL_H_