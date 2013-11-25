#ifndef _THEFOX_NET_SOCKET_H_
#define _THEFOX_NET_SOCKET_H_

#include <base/noncopyable.h>
#include <net/winapi.h>
#include <net/IoCompletionPort.h>
#include <net/InetAddress.h>
#include <net/IoBuffer.h>

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
	
	bool postAccept(IoCompletionIocp * const iocpPtr, IoBuffer *acceptIoContext)
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
	
	bool postSend(IoCompletionIocp * const iocpPtr, IoBuffer *ioContext)
	{
		DWORD dwFlags = 0;
		DWORD dwBytes = 0;

		ioContext->setIoType(IoBuffer::IoType::Send);

		int bytesRecv = ::WSASend(_socket, &ioContext->_wsaBuf, 1, &dwBytes, &dwFlags, &iocpPtr->getHandle(), NULL );

		if ((SOCKET_ERROR == bytesRecv) && (WSA_IO_PENDING != ::WSAGetLastError()))
		{
			return false;
		}
		return true;
	}
	
	bool postRecv(IoCompletionIocp * const iocpPtr, IoBuffer *ioContext)
	{
		DWORD dwFlags = 0;
		DWORD dwBytes = 0;

		ioContext->ResetBuffer();
		ioContext->setIoType(IoBuffer::IoType::Recv);

		int bytesRecv = ::WSARecv(_socket, &ioContext->_wsaBuf, 1, &dwBytes, &dwFlags, &iocpPtr->getHandle(), NULL );

		if ((SOCKET_ERROR == bytesRecv) && (WSA_IO_PENDING != ::WSAGetLastError()))
		{
			return false;
		}
		return true;
	}
	
	void getAcceptExSockAddrs(IoBuffer *acceptIoContext, InetAddress &localAddr, InetAddress &peerAddr)
	{
		SOCKADDR_IN* clientAddr = NULL;
		SOCKADDR_IN* serverAddr = NULL;
		int clientLen = sizeof(SOCKADDR_IN);
		int serverLen = sizeof(SOCKADDR_IN);
	
		_lpfnGetAcceptExSockAddrs(acceptIoContext->_wsaBuf.buf, 
			acceptIoContext->_wsaBuf.len - ((sizeof(SOCKADDR_IN)+16)*2),  
			sizeof(SOCKADDR_IN)+16, sizeof(SOCKADDR_IN)+16, 
			(LPSOCKADDR*)&serverAddr, &serverLen, 
			(LPSOCKADDR*)&clientAddr, &clientLen);
			
			localAddr = serverAddr;
			peerAddr = clientLen;
	}
	
	void close()
	{
		if (INVALID_SOCKET != _socket)
		{
			closesocket(_socket);
			_socket = INVALID_SOCKET;
		}
	}
	
	void setTcpNoDelay(bool on)
	{
		int optval = on ? 1 : 0;
		::setsockopt(_socket, IPPROTO_TCP, TCP_NODELAY, &optval, static_cast<socklen_t>(sizeof(optval)));
	}
	
	void setKeepAlive(bool on)
	{
		int optval = on ? 1 : 0;
		::setsockopt(_socket, SOL_SOCKET, SO_KEEPALIVE, &optval, static_cast<socklen_t>(sizeof(optval)));
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