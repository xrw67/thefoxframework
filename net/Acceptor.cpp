#include <net/InetAddress.h>
#include <net/Socket.h>
#include <net/Acceptor.h>
#include <net/IoContext.h>

using namespace thefox;

Acceptor::Acceptor(Socket *socket, const InetAddress &listenAddr)
	: _acceptSocket(socket)
	, _listening(false)
{
	_acceptSocket->addToIocp(this);

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
}

Acceptor::~Acceptor()
{
}

void Acceptor::listen()
{
	_listening = true;
	
	_acceptSocket->listen();
	
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


