#ifndef _THEFOX_TYPES_H_
#define _THEFOX_TYPES_H_

namespace thefox 
{

typedef long long int64_t

// 字符串使用std::string 还是 thefox::String
#ifdef _THEFOX_STD_STRING
#	include <string>
	typedef _STD string String;
#else
#	include <base/String.h>
#endif

#define Map _STD map
#define Vector _STD vector
#define List _STD vector
#define Bitset _STD bitset

} // namespace thefox

#endif // _THEFOX_TYPES_H_