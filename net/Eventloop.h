#ifndef _THEFOX_NET_EVENTLOOP_H_
#define _THEFOX_NET_EVENTLOOP_H_

#include <base/noncopyable.h>
#include <base/Types.h>

namespace thefox
{

class TcpServer;

class Eventloop : noncopyable
{
public:
	Eventloop(TcpServer *server);
	~Eventloop();
	
	void loop();
	void quit() { _quit = true; }
	
private:
	TcpServer *_server;//·þÎñÆ÷¾ä±ú
	bool _looping;
	bool _quit;
	const uint32_t _threadId;
};

}

#endif // _THEFOX_NET_EVENTLOOP_H_