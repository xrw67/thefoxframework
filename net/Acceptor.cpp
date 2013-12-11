#include <net/Acceptor.h>
#include <net/InetAddress.h>
#include <net/IoBuffer.h>

using namespace thefox;

Acceptor::Acceptor(const InetAddress &listenAddr)
	: _listening(false)
{
	_acceptSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	bind(_acceptSocket, (struct sockaddr *)&listenAddr.getSockAddrInet(), sizeof(listenAddr.getSockAddrInet()));

	DWORD dwBytes = 0;  
	GUID guidAcceptEx = WSAID_ACCEPTEX;  
	GUID guidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS; 
	::WSAIoctl(_acceptSocket, 
				SIO_GET_EXTENSION_FUNCTION_POINTER, 
				&guidAcceptEx, 
				sizeof(guidAcceptEx), 
				&_lpfnAcceptEx, 
				sizeof(_lpfnAcceptEx), 
				&dwBytes, 
				NULL, 
				NULL);
	::WSAIoctl(_acceptSocket, 
				SIO_GET_EXTENSION_FUNCTION_POINTER, 
				&guidGetAcceptExSockAddrs, 
				sizeof(guidGetAcceptExSockAddrs), 
				&_lpfnGetAcceptExSockAddrs, 
				sizeof(_lpfnGetAcceptExSockAddrs), 
				&dwBytes, 
				NULL, 
				NULL);
}

Acceptor::~Acceptor()
{
	_listening = false;
	while (_IoBuffers.empty()) {
		delete _IoBuffers.front();
		_IoBuffers.pop_front();
	}
}

void Acceptor::listen()
{
	_listening = true;
	::listen(_acceptSocket, SOMAXCONN);
}

void Acceptor::stop()
{
	_listening = false;
}

void Acceptor::getAcceptExSockAddrs(IoBuffer *ioBuffer, InetAddress &localAddr, InetAddress &peerAddr)
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

bool Acceptor::postAccept(IoBuffer *ioBuffer)
{
	DWORD dwBytes = 0;
	if ((ioBuffer->_socket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
		return false;
	
	if (FALSE == _lpfnAcceptEx(_acceptSocket, ioBuffer->_socket, ioBuffer->getWSABuffer()->buf, 0, 
				sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwBytes, &ioBuffer->_overlapped)) {
		if (WSA_IO_PENDING != WSAGetLastError())
			return false;
	}
	return true;
}