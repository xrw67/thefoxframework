#include <net/InetAddress.h>
#include <net/Acceptor.h>
#include <net/IoBuffer.h>

using namespace thefox;

Acceptor::Acceptor(const InetAddress &listenAddr)
	: _acceptSocket(Socket::create())
	, _listening(false)
{
	_acceptSocket.bindAddress(listenAddr);
}

Acceptor::~Acceptor()
{
	_listening = false;
	while (_acceptIoBuffers.empty()) {
		delete _acceptIoBuffers.front();
		_acceptIoBuffers.pop_front();
	}
}

void Acceptor::listen()
{
	_listening = true;
	
	_acceptSocket.listen();
	
	while (_acceptIoBuffers.size() < kMaxPostAccept) {
		IoBuffer *ioBuffer = new IoBuffer(IoType::Accept);
		if (_acceptSocket.postAccept(ioBuffer))
			_acceptIoBuffers.push_back(ioBuffer);
		else
			delete ioBuffer;
	}
}

void Acceptor::handleAccept(IoBuffer *ioBuffer)
{
	InetAddress localAddr;
	InetAddress peerAddr;
	_acceptSocket.getAcceptExSockAddrs(ioBuffer, localAddr, peerAddr);
	if (_newConnectionCallback)
		_newConnectionCallback(ioBuffer->_socket, localAddr, peerAddr);
	else
		closesocket(ioBuffer->_socket);
	
	ioBuffer->reset();
	_acceptSocket.postAccept(ioBuffer);
}
