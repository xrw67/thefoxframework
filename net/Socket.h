#ifndef _THEFOX_NET_SOCKET_H_
#define _THEFOX_NET_SOCKET_H_

#include <base/noncopyable.h>
#include <net/winapi.h>
#include <net/IoCompletionPort.h>
#include <net/InetAddress.h>
#include <net/IoContext.h>

namespace thefox
{

class Socket : noncopyable
{
public:
	static const SOCKET create()
	{ return ::WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED); }
	
	Socket(const SOCKET socket)
		: _socket(socket)
	{
		DWORD dwBytes = 0;  
		GUID guidAcceptEx = WSAID_ACCEPTEX;  
		GUID guidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS; 
		::WSAIoctl(_socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidAcceptEx, sizeof(guidAcceptEx), &_lpfnAcceptEx, sizeof(_lpfnAcceptEx), &dwBytes, NULL, NULL);
		::WSAIoctl(_socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidGetAcceptExSockAddrs, sizeof(guidGetAcceptExSockAddrs), &_lpfnGetAcceptExSockAddrs, sizeof(_lpfnGetAcceptExSockAddrs), &dwBytes, NULL, NULL);
	}
	~Socket()
	{
		close();
	}
	
	bool bindAddress(const InetAddress &listenAddr)
	{ ::bind(_socket, (struct sockaddr *)&listenAddr.getSockAddrInet(), sizeof(listenAddr.getSockAddrInet())); }
	
	bool listen();
	{ ::listen(_socket, SOMAXCONN); }
	
	bool connect(const InetAddress &addr)
	{ ::connect(_socket, (struct sockaddr *)&listenAddr.getSockAddrInet(), sizeof(listenAddr.getSockAddrInet())); }
	
	bool postAccept(IoCompletionIocp * const iocpPtr, IoContext *acceptIoContext)
	{
		DWORD dwBytes = 0;
		if ((acceptIoContext->_socket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
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
	
	bool postSend(IoCompletionIocp * const iocpPtr, IoContext *ioContext)
	{
		DWORD dwFlags = 0;
		DWORD dwBytes = 0;

		ioContext->setIoType(IoContext::IoType::Send);

		int bytesRecv = ::WSASend(_socket, &ioContext->_wsaBuf, 1, &dwBytes, &dwFlags, &iocpPtr->getHandle(), NULL );

		if ((SOCKET_ERROR == bytesRecv) && (WSA_IO_PENDING != ::WSAGetLastError()))
		{
			return false;
		}
		return true;
	}
	
	bool postRecv(IoCompletionIocp * const iocpPtr, IoContext *ioContext)
	{
		DWORD dwFlags = 0;
		DWORD dwBytes = 0;

		ioContext->ResetBuffer();
		ioContext->setIoType(IoContext::IoType::Recv);

		int bytesRecv = ::WSARecv(_socket, &ioContext->_wsaBuf, 1, &dwBytes, &dwFlags, &iocpPtr->getHandle(), NULL );

		if ((SOCKET_ERROR == bytesRecv) && (WSA_IO_PENDING != ::WSAGetLastError()))
		{
			return false;
		}
		return true;
	}
	
	void close()
	{
		if (INVALID_SOCKET != _socket)
		{
			closesocket(_socket);
			_socket = INVALID_SOCKET;
		}
	}
	
	const SOCKET getSocketHandle() const 
	{ return _socket; }
private:
	const SOCKET _socket;
	LPFN_ACCEPTEX _lpfnAcceptEx;
	LPFN_GETACCEPTEXSOCKADDRS _lpfnGetAcceptExSockAddrs; 
};

}

#endif //_THEFOX_NET_SOCKET_H_