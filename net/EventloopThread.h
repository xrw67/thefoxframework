#ifndef _THEFOX_NET_EVENTLOOPTHREAD_H_
#define _THEFOX_NET_EVENTLOOPTHREAD_H_

#include <base/noncopyable.h>

namespace thefox
{

class EventloopThread : noncopyable
{
public:
	void stratLoop();
private:
	static DWORD WINAPI threadProc(LPVOID lpParameter);

	Eventloop *_loop;
	bool _exiting;
};

}

#endif // _THEFOX_NET_EVENTLOOPTHREAD_H_