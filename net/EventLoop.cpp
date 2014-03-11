#include <net/EventLoop.h>
#include <net/Event.h>
#include <net/Iocp.h>

using namespace thefox;
using namespace thefox::net;

EventLoop::EventLoop(Iocp *iocp)
: _iocp(iocp)
, _quit(false)
{
}

EventLoop::~EventLoop()
{
    _quit = true;
}

void EventLoop::loop()
{
	Event *evt = NULL;

	while (!_quit) {
		evt = _iocp->poll();
		if (evt)
			evt->handleEvent();
	}
}