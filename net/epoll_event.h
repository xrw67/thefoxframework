#ifndef _THEFOX_NET_EPOLL_H_
#define _THEFOX_NET_EPOLL_H_

#include <vector>
#include <base/common.h>
#include <net/event.h>

#ifdef WIN32
#error "class EpollEvent run in Linux only"
#else

struct epoll_event;

namespace thefox
{

class TcpConnection;

class EpollEvent
{
	EpollEvent();
	~EpollEvent();

	bool init();
	bool processEvents(uint32_t timer);

    bool registerConnection(TcpConnection *conn);
    bool unregisterConnection(TcpConnection *conn);
    void closeConnection(TcpConnection *conn);
    bool updateRead(TcpConnection *conn);
    bool updateWrite(TcpConnection *conn);
	
private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(EpollEvent);
    bool handler(TcpConnection *conn, uint32_t revents);
    bool onRead(TcpConnection *conn);
    bool onWrite(TcpConnection *conn);

	typedef std::vector<struct epoll_event> EventList;
	int _epollfd;
	EventList _events;

	static const size_t kDefaultBufferSize = 8192;
};

} // namespace thefox

#endif // WIN32

#endif // _THEFOX_NET_EPOLL_H_
