#ifndef _THEFOX_NET_SOCKET_H_
#define _THEFOX_NET_SOCKET_H_

#include <base/noncopyable.h>
#include <net/winapi.h>
#include <net/InetAddress.h>
#include <net/IoBuffer.h>

namespace thefox
{

class Socket : noncopyable
{
public:
	static SOCKET create()
	{ return ::WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED); }
	
	explicit Socket(SOCKET socket)
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
	{ 
		if (SOCKET_ERROR == ::bind(_socket, (struct sockaddr *)&listenAddr.getSockAddrInet(), sizeof(listenAddr.getSockAddrInet()))) {
			return false;
		}
		return true;
	}
	
	bool listen()
	{ 
		if (SOCKET_ERROR == ::listen(_socket, SOMAXCONN)) {
			return false;
		}
		return true;
	}
	
	bool connect(const InetAddress &addr)
	{ 
		if (SOCKET_ERROR == ::connect(_socket, (struct sockaddr *)&addr.getSockAddrInet(), sizeof(addr.getSockAddrInet()))) {
			return false;
		}
		return true;
	}
	
	bool postAccept(IoBuffer *ioBuffer)
	{
		DWORD dwBytes = 0;
		if ((ioBuffer->_socket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
			return false;
	
		if (FALSE == _lpfnAcceptEx(_socket, ioBuffer->_socket, ioBuffer->getWSABuffer()->buf, 0, 
					sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwBytes, &ioBuffer->_overlapped)) {
			if (WSA_IO_PENDING != WSAGetLastError())
				return false;
		}
		return true;
	}
	
	bool postSend(IoBuffer *ioBuffer)
	{
		DWORD dwFlags = 0;
		DWORD dwBytes = 0;

		int bytesRecv = ::WSASend(_socket, ioBuffer->getWSABuffer(), 1, &dwBytes, dwFlags, &ioBuffer->_overlapped, NULL );

		if ((SOCKET_ERROR == bytesRecv) && (WSA_IO_PENDING != ::WSAGetLastError()))
		{
			return false;
		}
		return true;
	}
	
	bool postRecv(IoBuffer *ioBuffer)
	{
		DWORD dwFlags = 0;
		DWORD dwBytes = 0;

		int bytesRecv = ::WSARecv(_socket, ioBuffer->getWSABuffer(), 1, &dwBytes, &dwFlags, &ioBuffer->_overlapped, NULL );

		if ((SOCKET_ERROR == bytesRecv) && (WSA_IO_PENDING != ::WSAGetLastError()))
		{
			return false;
		}
		return true;
	}
	
	void getAcceptExSockAddrs(IoBuffer *ioBuffer, InetAddress &localAddr, InetAddress &peerAddr)
	{
		SOCKADDR_IN* clientAddr = NULL;
		SOCKADDR_IN* serverAddr = NULL;
		int clientLen = sizeof(SOCKADDR_IN);
		int serverLen = sizeof(SOCKADDR_IN);
	
		_lpfnGetAcceptExSockAddrs(ioBuffer->getWSABuffer()->buf,  ioBuffer->getWSABuffer()->len - ((sizeof(SOCKADDR_IN)+16)*2),
			sizeof(SOCKADDR_IN)+16, sizeof(SOCKADDR_IN)+16, (LPSOCKADDR*)&serverAddr, &serverLen, (LPSOCKADDR*)&clientAddr, &clientLen);
			
			localAddr.setSockAddrInet(*serverAddr);
			peerAddr.setSockAddrInet(*clientAddr);
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
		::setsockopt(_socket, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char *>(&optval), sizeof(optval));
	}
	
	void setKeepAlive(bool on)
	{
		int optval = on ? 1 : 0;
		::setsockopt(_socket, SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<char *>(&optval), sizeof(optval));
	}
	
	SOCKET getSocketHandle() { return _socket; }
private:
	SOCKET _socket;
	LPFN_ACCEPTEX _lpfnAcceptEx;
	LPFN_GETACCEPTEXSOCKADDRS _lpfnGetAcceptExSockAddrs; 
};

}

#endif //_THEFOX_NET_SOCKET_H_