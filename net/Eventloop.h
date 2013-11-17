#ifndef _THEFOX_NET_EVENTLOOP_H_
#define _THEFOX_NET_EVENTLOOP_H_

#include <base/noncopyable.h>

class IoCompletionPort;

namespace thefox
{

class Eventloop : noncopyable
{
public:
	Eventloop(IoCompletionPort *iocpPtr);
	~Eventloop();
	
	void loop();
	void quit();
	
private:
	IoCompletionPort * const _iocpPtr;

	bool _looping;
	bool _quit;
	const DWORD _threadId;
};

}

#endif // _THEFOX_NET_EVENTLOOP_H_