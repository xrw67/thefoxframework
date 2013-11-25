#include <net/InetAddress.h>
#include <net/Socket.h>
#include <net/Acceptor.h>
#include <net/IoBuffer.h>

using namespace thefox;

Acceptor::Acceptor(const InetAddress &listenAddr)
	: _acceptSocket(Socket::create())
	, _listening(false)
{
	setContextType(ContextType::Acceptor);
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
		IoBuffer *acceptIoContext = new IoBuffer(IoBuffer::IoType::Accept);
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

void Acceptor::handleAccept(IoBuffer *acceptIoContext)
{
	InetAddress localAddr;
	InetAddress peerAddr;
	_acceptSocket->getAcceptExSockAddr(acceptIoContext, localAddr, peerAddr);
	if (_newConnectionCallback)
	{
		_newConnectionCallback(acceptIoContext->_socket, localAddr, peerAddr);
	}
	else
	{
		closesocket(acceptIoContext->_socket);
	}
	
	acceptIoContext->resetBuffer();
	_acceptSocket->postAccept(acceptIoContext);
}


