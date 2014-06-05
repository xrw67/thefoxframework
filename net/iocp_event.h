#if defined WIN32 && !defined _THEFOX_NET_IOCP_EVENT_H_
#define _THEFOX_NET_IOCP_EVENT_H_

#include <vector>
#include <Winsock2.h>
#include <base/common.h>
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
	bool processEvents(uint32_t timer);
	bool postClose(const TcpConnectionPtr &conn);

	void registerConnection(const TcpConnectionPtr &conn);
	bool unregisterConnection(const TcpConnectionPtr &conn);

	bool updateRead(const TcpConnectionPtr &conn);
	bool updateWrite(const TcpConnectionPtr &conn);

private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(IocpEvent);
	void handler(const TcpConnectionPtr &conn, int32_t ovlpType, uint32_t avaliable);
	void handleRead(const TcpConnectionPtr &conn, uint32_t avaliable);
	void handleWrite(const TcpConnectionPtr &conn, uint32_t avaliable);

	HANDLE _hIocp;
	MemPool<EventOvlp> _ovlpPool;
	static const size_t kMaxBufSize = 8192;
};

} // namespace thefox

#endif // _THEFOX_NET_IOCP_EVENT_H_
