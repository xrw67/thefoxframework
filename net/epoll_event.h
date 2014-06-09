#ifndef _THEFOX_NET_EPOLL_EVENT_H_
#define _THEFOX_NET_EPOLL_EVENT_H_

#include <vector>
#include <base/common.h>
#include <net/event.h>

#ifdef WIN32
#error "class EpollEvent run in Linux only"
#else

struct epoll_event;

namespace thefox {
namespace net {

class TcpConnection;

class EpollEvent
{
	EpollEvent();
	~EpollEvent();

	bool init();
	bool processEvents(uint32_t timer);

    bool addEvent(Event *ev);
    bool delEvent(Event *ev);
    bool updateRead(Event *ev) { return true; }
    bool updateWrite(Event *ev);
	
private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(EpollEvent);
    bool handler(Event *ev, void *arg);
    bool onRead(Event *ev);
    bool onWrite(Event *ev);

	typedef std::vector<struct epoll_event> EventList;
	int _epollfd;
	EventList _events;

	static const size_t kDefaultBufferSize = 8192;
};

} // namespace net
} // namespace thefox

#endif // WIN32

#endif // _THEFOX_NET_EPOLL_EVENT_H_
