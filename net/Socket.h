#ifndef _THEFOX_NET_SOCKET_H_
#define _THEFOX_NET_SOCKET_H_

#include <net/winapi.h>
#include <net/InetAddress.h>
#include <net/IoBuffer.h>

namespace thefox
{

inline bool connect(SOCKET socket, const InetAddress &addr)
{ 
	if (SOCKET_ERROR == ::connect(socket, (struct sockaddr *)&addr.getSockAddrInet(), sizeof(addr.getSockAddrInet()))) {
		return false;
	}
	return true;
}
	

	
inline bool postSend(SOCKET socket, IoBuffer *ioBuffer)
{
	DWORD dwFlags = 0;
	DWORD dwBytes = 0;

	int bytesRecv = ::WSASend(socket, ioBuffer->getWSABuffer(), 1, &dwBytes, dwFlags, &ioBuffer->_overlapped, NULL );

	if ((SOCKET_ERROR == bytesRecv) && (WSA_IO_PENDING != ::WSAGetLastError()))
	{
		return false;
	}
	return true;
}
	
inline bool postRecv(SOCKET socket, IoBuffer *ioBuffer)
{
	DWORD dwFlags = 0;
	DWORD dwBytes = 0;

	int bytesRecv = ::WSARecv(socket, ioBuffer->getWSABuffer(), 1, &dwBytes, &dwFlags, &ioBuffer->_overlapped, NULL );

	if ((SOCKET_ERROR == bytesRecv) && (WSA_IO_PENDING != ::WSAGetLastError()))
	{
		return false;
	}
	return true;
}
	
inline void SocketClose(SOCKET socket)
{
	if (INVALID_SOCKET != socket)
	{
		closesocket(socket);
		socket = INVALID_SOCKET;
	}
}
	
inline void setTcpNoDelay(SOCKET socket, bool on)
{
	int optval = on ? 1 : 0;
	::setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char *>(&optval), sizeof(optval));
}
	
inline void setKeepAlive(SOCKET socket, bool on)
{
	int optval = on ? 1 : 0;
	::setsockopt(socket, SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<char *>(&optval), sizeof(optval));
}
	
}

#endif //_THEFOX_NET_SOCKET_H_