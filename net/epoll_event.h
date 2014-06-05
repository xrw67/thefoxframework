#ifndef _THEFOX_NET_EPOLL_H_
#define _THEFOX_NET_EPOLL_H_

#include <vector>
#include <base/common.h>
#include <net/event.h>

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
	bool postClose(const TcpConnectionPtr &conn);

	bool registerConnection(const TcpConnectionPtr &conn);
	bool unregisterConnection(const TcpConnectionPtr &conn);

	bool updateRead(const TcpConnectionPtr &conn);
	bool updateWrite(const TcpConnectionPtr &conn);
	
private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(EpollEvent);
	void handler(const TcpConnectionPtr &conn, uint32_t revents);
	void handleRead(const TcpConnectionPtr &conn);
	void handleWrite(const TcpConnectionPtr &conn);

	typedef std::vector<struct epoll_event> EventList;
	int _epollfd;
	EventList _events;
};

} // namespace thefox

#endif // _THEFOX_NET_EPOLL_H_
