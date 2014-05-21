#include <net/tcp_connection.h>
#include <net/tcp_handler.h>
#include <net/event_loop.h>

using namespace thefox;

TcpConnection::TcpConnection(TcpHandler *handler, EventLoop *loop, SOCKET sockfd,
							int connId, const InetAddress &peerAddr)
{
	: _handler(handler)
	, _loop(loop)
    , _id(id)
    , _peerAddr(peerAddr)
    , _readBytes(0)
    , _writeBytes(0)
	, _arg(NULL)
{
	_socket.setFd(sockfd);

	memset(&_ev, 0, sizeof(_ev));
	_ev.conn = this;
}

TcpConnection::~TcpConnection()
{
}

void TcpConnection::send(const std::string &data)
{
	this->send(data.c_str(), data.length());
}

void TcpConnection::send(const char *data, size_t len)
{
	_writeBuffer.append(data, len);

	MutexGuarg lock(_ev._mutex);
	if (!ev->write) {
		ev->write = true;
		_loop->postEvent(&ev);
	}
}

void TcpConnection::destroy()
{
	_loop->delConnection(this);
	if (0 == --_ev.ref) {
		delete this;
	}
}