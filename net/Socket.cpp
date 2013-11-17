#include <net/Socket.h>
#include <net/IoContext.h>

using namespace thefox;

Socket::Socket(IoCompletionPort *iocp)
	: _iocp(iocp)
	, _socket(INVALID_SOCKET)
{}

Socket::~Socket()
{
	if (INVALID_SOCKET != _socket)
	{
		closesocket(_socket);
	}
}
void Socket::create()
{
	_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
}

bool Socket::addToIocp(ULONG_PTR completionKey)
{
	_iocp->addHandle((HANDLE)_socket, completionKey);
}

bool Socket::bind(const InetAddress &listenAddr)
{
	if (SOCKET_ERROR == bind(_socket, 
			(struct sockaddr *)&listenAddr.getSockAddrInet(),
			sizeof(listenAddr.getSockAddrInet())))
	{
		// failed;
	}
}

bool Socket::postAccept()
{
	DWORD dwBytes = 0;
	if ((acceptIoContext->_socket = 
			WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
	{
		return false;
	}
	
	if (FALSE == _lpfnAcceptEx(_socket, acceptIoContext->_socket, acceptIoContext->_wsaBuf.buf, 0, 
				sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwBytes, _iocp.getHandle()))
	{
		if (WSA_IO_PENDING != WSAGetLastError())
		{
			return false;
		}
	}
	return true;
}

