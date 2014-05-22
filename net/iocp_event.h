#if defined WIN32 && !defined _THEFOX_NET_IOCP_EVENT_H_
#define _THEFOX_NET_IOCP_EVENT_H_

#include <vector>
#include <Winsock2.h>
#include <base/types.h>

namespace thefox
{

class IoEvent;
class TcpConnection;

class IocpEvent
{
public:
	IocpEvent();
	~IocpEvent();

	bool init();
	bool addEvent(IoEvent *ev);
	void addConnection() {}
	bool delConnection(TcpConnection *conn);
	bool processEvents(int32_t timer);

	bool postRead(IoEvent *ev);
	bool postWrite(IoEvent *ev);

private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(IocpEvent);
	void handler(IoEvent *ev);
	void handleRead(IoEvent *ev);
	void handleWrite(IoEvent *ev);
	void handleClose(IoEvent *ev);
	
	HANDLE _hIocp;
	static const size_t kMaxBufSize = 8192;
};

} // namespace thefox

#endif // _THEFOX_NET_IOCP_EVENT_H_
