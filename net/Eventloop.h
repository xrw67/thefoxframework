#ifndef _THEFOX_NET_EVENTLOOP_H_
#define _THEFOX_NET_EVENTLOOP_H_

#include <base/noncopyable.h>

namespace thefox
{

class Eventloop : noncopyable
{
public:
	Eventloop(const HANDLE &completionPort);
	~Eventloop();
	
	void loop();
	void quit();
	
private:
	bool _looping;
	bool _quit;
	const DWORD _threadId;
	const HANDLE _completionPort;
};

}

#endif // _THEFOX_NET_EVENTLOOP_H_