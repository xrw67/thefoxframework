#ifndef _THEFOX_NET_ACCEPT_IOBUFFER_H_
#define _THEFOX_NET_ACCEPT_IOBUFFER_H_

#include <net/PerIoContext.h>

namespace thefox
{

#define MAX_ACCEPT_IO_BUFFER_SIZE 512

struct AcceptIoBuffer
{
	OVERLAPPED _overlapped;
	SOCKET _socket;
	char _buf[MAX_ACCEPT_IO_BUFFER_SIZE];
};
 
}

#endif // _THEFOX_NET_ACCEPT_IOBUFFER_H_