#ifndef _THEFOX_TYPES_H_
#define _THEFOX_TYPES_H_

#include <string>

namespace thefox 
{

typedef char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;
    
typedef std::string String;

#ifdef WIN32
#pragma warning(disable:4996)
#define snprintf _snprintf
#else

#endif

} // namespace thefox

#endif // _THEFOX_TYPES_H_