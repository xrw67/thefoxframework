#include <net/tcp_connection.h>
#include <log/logging.h>
#include <net/event_loop.h>

using namespace thefox;

TcpConnection::TcpConnection(EventLoop *loop, SOCKET sockfd,
							int id, const InetAddress &peerAddr)
	: _loop(loop)
    , _id(id)
    , _peerAddr(peerAddr)
    , _readBytes(0)
    , _writeBytes(0)
	, _arg(NULL)
	, _state(kConnecting)
{
	_socket.setFd(sockfd);
	_event.init(this);
}

TcpConnection::~TcpConnection()
{
	_arg = NULL;
	_id = -1;
}

void TcpConnection::send(const std::string &data)
{
	this->send(data.c_str(), data.length());
}

void TcpConnection::send(const char *data, size_t len)
{
	_writeBuffer.append(data, len);

	MutexGuard lock(_mutex);
	if (!_event.write) {
		_event.write = true;
		_loop->postEvent(&_event);
	}
}

bool TcpConnection::shutdown()
{
	return _socket.shutdownWrite();
}

void TcpConnection::setTcpNoDelay(bool on)
{
	_socket.setTcpNoDelay(on);
}

void TcpConnection::forceClose()
{
	if (kConnected == _state || kDisconnecting == _state) {
		setState(kDisconnecting);
		// 投递关闭事件
		_event.close = true;
		_loop->postEvent(&_event);
	}
}

void TcpConnection::connectEstablished()
{
	THEFOX_LOG(DEBUG) << "TcpConnection::connectEstablished()";

	assert(kConnecting == _state );
	setState(kConnected);
	_connectionCallback(this);

	_loop->addEvent(&_event);

	// 投递读事件
	_event.read = true;
	_loop->postEvent(&_event);
}

void TcpConnection::connectDestroyed()
{
	if (kConnected == _state || kDisconnecting == _state) {
		setState(kDisconnected);
		THEFOX_LOG(DEBUG) << "connection closed, addr:" << _peerAddr.toIpPort();

		_loop->delConnection(this);
		_socket.close();

		_connectionCallback(this);
	}

	if (0 == _event.refCount()) {
		_removeConnectionCallback(this); // 这是最后一行
		return;
	}
}
