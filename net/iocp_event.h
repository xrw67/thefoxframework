#if defined WIN32 && !defined _THEFOX_NET_IOCP_EVENT_H_
#define _THEFOX_NET_IOCP_EVENT_H_

#include <vector>
#include <Winsock2.h>
#include <base/types.h>

namespace thefox
{

class TcpConnection;

class IocpEvent
{
public:
	IocpEvent();
	~IocpEvent();

	bool init();
	void addEvent();
	void addConnection() {}
	bool delConnection(TcpConnection *conn);
	bool processEvents(int32_t timer);

private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(IocpEvent);

	HANDLE _hIocp;
};

} // namespace thefox

#endif // _THEFOX_NET_IOCP_EVENT_H_
