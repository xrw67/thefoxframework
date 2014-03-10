#include <net/EventLoop.h>
#include <net/Event.h>
#include <net/Iocp.h>

using namespace thefox;
using namespace thefox::net;

EventLoop::EventLoop(Iocp *iocp)
: _iocp(iocp)
{
}

EventLoop::~EventLoop()
{
}

void EventLoop::loop()
{
	Event *evt = NULL;

	while (true) {
		evt = _iocp->poll(1000);
		if (evt)
			evt->handleEvent();
	}
}