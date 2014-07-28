#ifndef _THEFOX_BASE_SYS_UTIL_H_
#define _THEFOX_BASE_SYS_UTIL_H_

#ifdef WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#else
    #include <unistd.h>
#endif 

#include <base/common.h>

namespace thefox {

class SysUtil
{
public:
	static void msleep(uint32_t msec)
    {
#ifdef WIN32
    	::Sleep(msec);
#else
    	usleep(msec * 1000);
#endif
    }
};

} // namespace thefox

#endif // _THEFOX_BASE_SYS_UTIL_H_