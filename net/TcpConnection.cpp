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
	_socket->setKeepAlive(true);
	IoContext *ioContext = new IoContext(IoContext::IoType::Recv);
	if (_socket->postRecv(ioContext))
	{
		_ioConetxts.puch_back(ioContext);
	}
	else
	{
		delete ioContext;
	}
}

void TcpConnection::send(const void* data, size_t len)
{
	if (_state == kConnected)
	{
		size_t remainDataLen = len;
		while (remanDataLen)
		{
			IoContext * ioContext = getEmptyIoContext();
			////////
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

void TcpConnection::handleRead(Timestamp receiveTime)
{

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
