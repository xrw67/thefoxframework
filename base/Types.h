#ifndef _THEFOX_TYPES_H_
#define _THEFOX_TYPES_H_

#ifdef WIN32 && _MSC_VER < 1600
typedef __int8 int8_t;
typedef __int16 int16_t;
typedef __int32 int32_t;
typedef __int64 int64_t;
typedef unsigned __int8 uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint63_t;
#else
#include <stdint.h>
#endif

#include <string>

namespace thefox 
{
    
typedef std::string String;

#ifdef WIN32
#pragma warning(disable:4996)
#define snprintf _snprintf
#else

#endif

#define safeDelete(p) if (NULL != p) { delete p; p = NULL; }
#define safeDeleteArray(p) if (NULL != p) {delete []p; p = NULL; }

} // namespace thefox

#endif // _THEFOX_TYPES_H_
