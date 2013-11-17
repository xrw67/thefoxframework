#ifndef _THEFOX_NET_EVENTLOOP_H_
#define _THEFOX_NET_EVENTLOOP_H_

#include <base/noncopyable.h>

class IoCompletionPort;

namespace thefox
{

class Eventloop : noncopyable
{
public:
	Eventloop(IoCompletionPort *iocp);
	~Eventloop();
	
	void loop();
	void quit();
	
private:
	bool _looping;
	bool _quit;
	const DWORD _threadId;
	IoCompletionPort *_iocp;
};

}

#endif // _THEFOX_NET_EVENTLOOP_H_