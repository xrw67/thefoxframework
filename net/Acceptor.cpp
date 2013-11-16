#include <net/Acceptor.h>

using namespace thefox

Acceptor::Acceptor(const HANDLE completionPort, const InetAddress &listenAddr)
	: _completionPort(compoetionPort)
	: _socket(WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED))
	, _listening(false)
{
	GUID guidAcceptEx = WSAID_ACCEPTEX;  
	GUID guidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS; 
	
	// 获取AcceptEx函数指针
	DWORD dwBytes = 0;  
	if (SOCKET_ERROR == WSAIoctl(_socket, SIO_GET_EXTENSION_FUNCTION_POINTER, 
			&guidAcceptEx, sizeof(guidAcceptEx), 
			&_lpfnAcceptEx, sizeof(_lpfnAcceptEx), 
			&dwBytes, NULL, NULL))  
	{  
		// failed
	}  
	
	// 获取GetAcceptExSockAddrs函数指针，也是同理
	if (SOCKET_ERROR == WSAIoctl(_socket, SIO_GET_EXTENSION_FUNCTION_POINTER, 
			&guidGetAcceptExSockAddrs, sizeof(guidGetAcceptExSockAddrs), 
			&_lpfnGetAcceptExSockAddrs, sizeof(_lpfnGetAcceptExSockAddrs),   
			&dwBytes, NULL, NULL))  
	{  
		// failed
	}
	
	if (SOCKET_ERROR == ::bind(
								_socket, 
								(struct sockaddr *)&listenAddr.getSockAddrInet(),
								sizeof(listenAddr.getSockAddrInet())))
	{
		// failed;
	}
}

Acceptor::~Acceptor()
{
	
	closesocket(_socket);
	_socket = INVALID_SOCKET;
}

Acceptor::listen()
{
	_listening = true;
	
	if (SOCKET_ERROR == listen(_socket, SOMAXCONN))
	{
		// failed
	}
	
	while (_acceptIoContexts.size() < kMaxPostAccept)
	{
		IoContext *acceptIoContext = new IoContext(IoContext::IoType::Accept);
		if (postAccept(acceptIoContext))
		{
			_acceptIoContexts.push_back(acceptIoContext);
		}
		else
		{
			delete acceptIoContext;
		}
	}
}

void Acceptor::handleAccept(IoContext *acceptIoContext)
{
	SOCKADDR_IN* ClientAddr = NULL;
	SOCKADDR_IN* LocalAddr = NULL;
	int remoteLen = sizeof(SOCKADDR_IN);
	localLen = sizeof(SOCKADDR_IN);
	localLen = sizeof(SOCKADDR_IN);
	
	_lpfnGetAcceptExSockAddrs(acceptIoContext->_wsaBuf.buf, acceptIoContext->_wsaBuf.len - ((sizeof(SOCKADDR_IN)+16)*2),  
		sizeof(SOCKADDR_IN)+16, sizeof(SOCKADDR_IN)+16, (LPSOCKADDR*)&LocalAddr, &localLen, (LPSOCKADDR*)&ClientAddr, &remoteLen);  

	if (_newConnectionCallback)
	{
		InetAddress peerAddr(*ClientAddr);
		_newConnectionCallback(acceptIoContext->_socket, peerAddr);
	}
	else
	{
		closesocket(acceptIoContext->_socket);
	}
	
	acceptIoContext->resetBuffer();
	postAccept(acceptIoContext);
}

bool Acceptor::postAccept(IoContext *acceptIoContext)
{
	DWORD dwBytes = 0;
	if ((acceptIoContext->_socket = 
			WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
	{
		return false;
	}
	
	if (FALSE == _lpfnAcceptEx(_socket, acceptIoContext->_socket, acceptIoContext->_wsaBuf.buf, 0, 
				sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwBytes, _completionPort))
	{
		if (WSA_IO_PENDING != WSAGetLastError())
		{
			return false;
		}
	}
	return true;
}
