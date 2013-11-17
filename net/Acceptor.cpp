#include <net/InetAddress.h>
#include <net/Socket.h>
#include <net/Acceptor.h>
#include <net/IoContext.h>

using namespace thefox;

Acceptor::Acceptor(const InetAddress &listenAddr)
	: _acceptSocket(Socket::create())
	, _listening(false)
{
	_iocpPtr->assocHandle(_acceptSocket->getSocket(), this);
	_acceptSocket->bindAddress(listenAddr);
}

Acceptor::~Acceptor()
{
	_acceptIoContexts.clear();
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


