#ifndef _THEFOX_NET_ACCEPT_IOBUFFER_H_
#define _THEFOX_NET_ACCEPT_IOBUFFER_H_

#include <net/PerIoContext.h>

namespace thefox
{
class AcceptIoBuffer : public PerIoContext
{
public:
	SOCKET _socket;
};

}

#endif // _THEFOX_NET_ACCEPT_IOBUFFER_H_