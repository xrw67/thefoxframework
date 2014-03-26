#ifndef _THEFOX_NET_EVENTLOOP_H_
#define _THEFOX_NET_EVENTLOOP_H_

#include <Windows.h>

namespace thefox
{

class IoEvent;

class EventLoop
{
public:
	EventLoop();
	~EventLoop();
	void exec(); // mutil thread
	void loop(); // don'y use, one thread
	void quit();
	void registerHandle(HANDLE h);
private:
	int getCpuNum();
	HANDLE _hIocp;
	HANDLE _hQuitEvent;
};

} // namespace thefox

#endif // _THEFOX_NET_EVENTLOOP_H_