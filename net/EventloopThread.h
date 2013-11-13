#ifndef _THEFOX_NET_EVENTLOOPTHREAD_H_
#define _THEFOX_NET_EVENTLOOPTHREAD_H_

#include <base/noncopyable.h>

namespace thefox
{

class EventloopThread : noncopyable
{
public:
	EventloopThread(TcpServer *server)
	void stratLoop();
private:
	static DWORD WINAPI threadProc(LPVOID lpParameter);

	Eventloop *_loop;
	bool _exiting;
	TcpServer *_server;
};

}

#endif // _THEFOX_NET_EVENTLOOPTHREAD_H_