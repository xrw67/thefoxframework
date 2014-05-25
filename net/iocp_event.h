#if defined WIN32 && !defined _THEFOX_NET_IOCP_EVENT_H_
#define _THEFOX_NET_IOCP_EVENT_H_

#include <vector>
#include <Winsock2.h>
#include <base/types.h>
#include <base/mem_pool.h>
#include <net/event.h>

namespace thefox
{

class TcpConnection;

class IocpEvent
{
public:
	IocpEvent();
	~IocpEvent();

	bool init();
	bool addEvent(IoEvent *ev);
	bool postClose(IoEvent *ev);
	void addConnection() {}
	bool delConnection(TcpConnection *conn);
	bool processEvents(uint32_t timer);

	bool updateRead(IoEvent *ev);
	bool updateWrite(IoEvent *ev);
private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(IocpEvent);
	void handler(IoEvent *ev, int32_t ovlpType);
	void handleRead(IoEvent *ev);
	void handleWrite(IoEvent *ev);
	void handleClose(IoEvent *ev);

	HANDLE _hIocp;
	MemPool<EventOvlp> _ovlpPool;
	static const size_t kMaxBufSize = 8192;
};

} // namespace thefox

#endif // _THEFOX_NET_IOCP_EVENT_H_
