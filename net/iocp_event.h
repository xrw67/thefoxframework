#ifndef _THEFOX_NET_IOCP_EVENT_H_
#define _THEFOX_NET_IOCP_EVENT_H_

#ifdef WIN32

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

    bool registerConnection(TcpConnection *conn);
    bool unregisterConnection(TcpConnection *conn);
    bool updateRead(TcpConnection *conn);
    bool updateWrite(TcpConnection *conn);

private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(IocpEvent);
    void handler(TcpConnection *conn, int32_t ovlpType, uint32_t avaliable);
    void onRead(TcpConnection *conn, uint32_t avaliable);
    void onWrite(TcpConnection *conn, uint32_t avaliable);

	HANDLE _hIocp;
	MemPool<EventOvlp> _ovlpPool;
	static const size_t kMaxBufSize = 8192;
};

} // namespace thefox

#else 
    #error "class IocpEvent run in Windows only"
#endif // WIN32

#endif // _THEFOX_NET_IOCP_EVENT_H_
