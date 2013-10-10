#ifndef _THEFOX_CURRENTTHREAD_H_
#define _THEFOX_CURRENTTHREAD_H_

#include <base/noncopyable.h>

namespace thefox
{
class CurrentThread : noncopyable
{
	static int tid() { return static_cast<int>(::GetCurrentThreadId()); }
	static String tidString()
	{
		char buf[32] = {0};
		snprintf(buf, buf[32], "%d", static_cast<int>(::GetCurrentThreadId()));
		return buf;
	}
};

}

#endif