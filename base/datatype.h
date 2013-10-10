#ifndef _THEFOX_DATATYPE_H_
#define _THEFOX_DATATYPE_H_

namespace thefox
{

typedef INT64 int64_t

// 字符串使用std::string 还是 thefox::String
#ifdef THEFOX_STD_STRING
	#include <string>
	typedef std::string String;
#else
	#include <base/String.h>
#endif

}

#endif // _THEFOX_DATATYPE_H_