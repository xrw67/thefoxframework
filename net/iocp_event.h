#ifndef _THEFOX_NET_IOCP_EVENT_H_
#define _THEFOX_NET_IOCP_EVENT_H_

#ifdef WIN32

#include <vector>
#include <Winsock2.h>
#include <base/common.h>
#include <base/mem_pool.h>
#include <net/event.h>

namespace thefox {
namespace net {

class TcpConnection;

class IocpEvent
{
public:
	IocpEvent();
	~IocpEvent();

	bool init();
	bool processEvents(uint32_t timer);

    bool addEvent(Event *ev);
    bool delEvent(Event *ev);
    bool updateRead(Event *ev);
    bool updateWrite(Event *ev);

private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(IocpEvent);
    void handler(Event *ev, void *ovlp);
    void onRead(Event *ev, uint32_t avaliable);
    void onWrite(Event *ev, uint32_t avaliable);

	HANDLE _hIocp;
	MemPool<EventOvlp> _ovlpPool;
	static const size_t kMaxBufSize = 8192;
};

} // namespace net
} // namespace thefox

#else 
    #error "class IocpEvent run in Windows only"
#endif // WIN32

#endif // _THEFOX_NET_IOCP_EVENT_H_
