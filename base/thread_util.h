#ifndef _THEFOX_BASE_THREAD_UTIL_H_
#define _THEFOX_BASE_THREAD_UTIL_H_

#ifdef WIN32
#else
#include <pthread.h>
#endif

#include <base/common.h>

namespace thefox
{

#ifdef WIN32
	#define	currentThreadId (uint32_t)::GetCurrentThreadId
#else
	#define currentThreadId (uint32_t)pthread_self
#endif

} // namespace thefox

#endif // _THEFOX_BASE_THREAD_UTIL_H_
