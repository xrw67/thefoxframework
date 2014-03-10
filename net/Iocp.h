#ifndef _THEFOX_NET_IOCP_H_
#define _THEFOX_NET_IOCP_H_

#include <net/win32.h>

namespace thefox
{
namespace net
{

class Event;

class Iocp
{
public:
	Iocp(void);
	~Iocp(void);

	/// @brief associate a socket to this iocp;
	bool associateSocket(SOCKET s, ULONG_PTR CompletionKey);

	Event *poll(int timeout);
private:
	HANDLE _hIocp;
};

} // namespace net
} // namespace thefox

#endif // _THEFOX_NET_IOCP_H_