#include <net/InetAddress.h>
#include <net/Acceptor.h>
#include <net/AcceptIoBuffer.h>

using namespace thefox;

Acceptor::Acceptor(const InetAddress &listenAddr)
	: _acceptSocket(Socket::create())
	, _listening(false)
{
	setContextType(ContextType::Acceptor);
	_acceptSocket.bindAddress(listenAddr);
}

Acceptor::~Acceptor()
{
	_acceptIoBuffer.clear();
}

void Acceptor::listen()
{
	_listening = true;
	
	_acceptSocket.listen();
	
	while (_acceptIoBuffer.size() < kMaxPostAccept) {
		AcceptIoBuffer *acceptIoBuffer = new AcceptIoBuffer();
		if (_acceptSocket.postAccept(*acceptIoBuffer))
			_acceptIoBuffer.push_back(acceptIoBuffer);
		else
			delete acceptIoBuffer;
	}
}

void Acceptor::handleAccept(AcceptIoBuffer *acceptIoBuffer)
{
	InetAddress localAddr;
	InetAddress peerAddr;
	_acceptSocket.getAcceptExSockAddrs(*acceptIoBuffer, localAddr, peerAddr);
	if (_newConnectionCallback)
		_newConnectionCallback(acceptIoBuffer->_socket, localAddr, peerAddr);
	else
		closesocket(acceptIoBuffer->_socket);
	
	acceptIoBuffer->resetBuffer();
	_acceptSocket.postAccept(*acceptIoBuffer);
}


