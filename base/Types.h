#ifndef _THEFOX_TYPES_H_
#define _THEFOX_TYPES_H_

#include <stdint.h>
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
