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
		if (_acceptSocket->postAccept(_iocpPtr, acceptIoContext))
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
	InetAddress peerAddr;
	_acceptSocket->getAcceptExSockAddr(acceptIoContext, peerAddr);
	if (_newConnectionCallback)
	{
		_newConnectionCallback(acceptIoContext->_socket, peerAddr);
	}
	else
	{
		closesocket(acceptIoContext->_socket);
	}
	
	acceptIoContext->resetBuffer();
	_acceptSocket->postAccept(acceptIoContext);
}


