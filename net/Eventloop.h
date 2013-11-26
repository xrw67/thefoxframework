#ifndef _THEFOX_NET_EVENTLOOP_H_
#define _THEFOX_NET_EVENTLOOP_H_

#include <base/noncopyable.h>
#include <base/Types.h>

namespace thefox
{

class IoCompletionPort;

class Eventloop : noncopyable
{
public:
	Eventloop(IoCompletionPort &iocp);
	~Eventloop();
	
	void loop();
	void quit() { _quit = true; }
	
private:
	IoCompletionPort &_iocp;

	bool _looping;
	bool _quit;
	const uint32_t _threadId;
};

}

#endif // _THEFOX_NET_EVENTLOOP_H_