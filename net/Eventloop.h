#ifndef _THEFOX_NET_EVENTLOOP_H_
#define _THEFOX_NET_EVENTLOOP_H_

#include <base/noncopyable.h>

namespace thefox
{

class Eventloop : noncopyable
{
public:
	Eventloop(const TcpServer *server);
	~Eventloop();
	
	void loop();
	void quit();
	
private:
	bool _looping;
	bool _quit;
	const DWORD _threadId;
	const TcpServer *_server;
};

}

#endif // _THEFOX_NET_EVENTLOOP_H_