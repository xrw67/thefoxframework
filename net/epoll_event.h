#ifndef _THEFOX_NET_EPOLL_H_
#define _THEFOX_NET_EPOLL_H_

#include <base/common.h>
#include <net/event.h>

namespace thefox
{
	
class TcpConnection;

class EpollEvent
{
	EpollEvent();
	~EpollEvent();

	bool init();
	bool addEvent(IoEvent *ev);
	bool postClose(IoEvent *ev);
	void addConnection();
	bool delConnection(TcpConnection *conn);
	bool processEvents(uint32_t timer);

	bool updateRead(IoEvent *ev);
	bool updateWrite(IoEvent *ev);
private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(EpollEvent);
	void handler(IoEvent *ev);


};
} // namespace thefox
#endif // _THEFOX_NET_EPOLL_H_
