#include <net/TcpConnection.h>
#include <net/Socket.h>
using namespace thefox;

TcpConnection::TcpConnection(const String &name, 
							SOCKET socket, 
							const InetAddress &localAddr, 
							const InetAddress &peerAddr)
	: _name(name)
	, _socket(socket)
	, _localAddr(localAddr)
	, _peerAddr(peerAddr)
{	
	_socket->setKeepAlive(true);
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
	if (_state == kConnected) {
		setState(kDisconnecting);
		....
	}
		
}

void TcpConnection::setTcpNoDelay(bool on)
{
  socket_->setTcpNoDelay(on);
}

void TcpConnection::connectEstablished()
{
	setState(kConnected);
 
	IoBuffer *ioBuffer = new IoBuffer(IoType::Recv);
	if (_socket->postRecv(ioBuffer))
		_ioConetxts.puch_back(ioBuffer);
	else
		delete ioBuffer;

	connectionCallback_(this);
}

void TcpConnection::connectDestroyed()
{
	if (state_ == kConnected)
	{
		setState(kDisconnected);
		
		connectionCallback_(this);
  }
}

void TcpConnection::handleIoProcess(IoBuffer *iobuf, DWORD bytesTransfered)
{
	if (NULL == iobuf)
		return;
	
	switch (iobuf->getIoType()) {
		case IoType::Init:
			break;
		case IoType::Read:
			handleRead(iobuf);
			break;
		case IoType::ReadCompleted:
			handleCompleted(iobuf);
			break;
		case IoType::Write:
			handleWrite(iobuf);
			break;
		case IoType::WriteCompleted:
			handleWriteCompleted(iobuf);
			break;
		case IoType::ZeroByteRead:
			handleZeroByteRead(iobuf);
			break;
		case IoType::ZeroReadCompleted:
			handleZeroByteReadCompleted(iobuf);
			break;
		default:
			break;
	}
	
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
	setState(kDisconnected);
	connectionCallback_(this);
	closeCallback_(this);
}

void TcpConnection::handleError()
{
	
}
