#include <net/TcpConnection.h>
#include <net/Socket.h>
using namespace thefox;

TcpConnection::TcpConnection(const String &name, 
							SOCKET socket, 
							const InetAddress &localAddr, 
							const InetAddress &peerAddr)
	: _name(name)
	, _socket(new Socket(socket))
	, _localAddr(localAddr)
	, _peerAddr(peerAddr)
{
	setContextType(ContextType::TcpConnection);
	
	_socket->setKeepAlive(true);
	IoBuffer *ioBuffer = new IoBuffer(IoBuffer::IoType::Recv);
	if (_socket->postRecv(ioBuffer))
		_ioConetxts.puch_back(ioBuffer);
	else
		delete ioBuffer;
}

void TcpConnection::send(const void* data, size_t len)
{
	if (_state == kConnected) {
		size_t remain = len;
		while (remain) {
            size_t sendLen = (remain > IoBuffer::kMaxBufLen) ? IoBuffer::kMaxBufLen : remain;
			IoBuffer * ioBuffer = getFreeIoContext();
			ioBuffer->setIoType(IoBuffer::IoType::Send);
            ioBuffer->setBuffer(data, sendLen);
            remain -= sendLen;
            data += sendLen;
		}
	}
}

void TcpConnection::shutdown()
{

}

void TcpConnection::setTcpNoDelay(bool on)
{
  socket_->setTcpNoDelay(on);
}

void TcpConnection::handleRead(IoBuffer *ioBuffer, Timestamp receiveTime)
{
    _inBuffer.readIoContext(*ioBuffer);
    _messageCallback(this, _inBuffer, receiveTime);
    
    ioBuffer->resetBuffer();
    _socket->postRecv(ioBuffer)
}

void TcpConnection::handleWrite()
{

}

void TcpConnection::handleClose()
{

}

void TcpConnection::handleError()
{
	
}
