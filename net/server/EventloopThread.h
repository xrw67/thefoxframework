#ifndef _THEFOX_NET_EVENTLOOPTHREAD_H_
#define _THEFOX_NET_EVENTLOOPTHREAD_H_

#include <base/noncopyable.h>


namespace thefox
{

class IoCompletionPort;

class EventloopThread : noncopyable
{
public:
	EventloopThread()
	void stratLoop(IoCompletionPort * const iocpPtr);
private:
	static DWORD WINAPI threadProc(LPVOID);

	Eventloop *_loop;
	bool _exiting;
};

}

#endif // _THEFOX_NET_EVENTLOOPTHREAD_H_