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

	void postRead(IoEvent *ev);
	void poseWrite(IoEvent *ev);
	void poseZeroByteRead(IoEvent *ev);

private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(IocpEvent);
	void handleRead(IoEvent *ev);
	void handleWrite(IoEvent *ev);
	void handleClose(IoEvent *ev);
	void handleError(IoEvent *ev);
	void handleZeroByteRead(IoEvent *ev);
	
	HANDLE _hIocp;
};

} // namespace thefox

#endif // _THEFOX_NET_IOCP_EVENT_H_
