#include <net/tcp_connection.h>
#include <log/logging.h>
#include <net/event_loop.h>

using namespace thefox;

void thefox::defaultMessageCallback(TcpConnection *conn,
                                Buffer* buf,
                                Timestamp recvTime)
{
	buf->retrieveAll();
}

TcpConnection::TcpConnection(EventLoop *loop, SOCKET sockfd, int id, 
			const InetAddress& localAddr, const InetAddress &peerAddr)
	: _loop(loop)
	, _socket(sockfd)
    , _id(id)
	, _localAddr(localAddr)
    , _peerAddr(peerAddr)
    , _readBytes(0)
    , _writeBytes(0)
	, _arg(NULL)
	, _state(kConnecting)
{
	THEFOX_TRACE_FUNCTION;

	_event.init(this);
}

TcpConnection::~TcpConnection()
{
	THEFOX_TRACE_FUNCTION;
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
		_loop->updateWrite(&_event);
	}
}

bool TcpConnection::shutdown()
{
	THEFOX_TRACE_FUNCTION;

	return _socket.shutdownWrite();
}

void TcpConnection::setTcpNoDelay(bool on)
{
	THEFOX_TRACE_FUNCTION << "on=" << on;

	_socket.setTcpNoDelay(on);
}

void TcpConnection::forceClose()
{
	THEFOX_TRACE_FUNCTION;

	if (kConnected == _state || kDisconnecting == _state) {
		setState(kDisconnecting);
		// 投递关闭事件
		_event.close = true;
		_loop->postClose(&_event);
	}
}

void TcpConnection::connectEstablished()
{
	THEFOX_TRACE_FUNCTION;

	assert(kConnecting == _state );
	setState(kConnected);

	_loop->addEvent(&_event);

	// 投递读事件
	_event.read = true;
	_loop->updateRead(&_event);

	if (_connectionCallback)
		_connectionCallback(this);
}

void TcpConnection::connectDestroyed()
{
	THEFOX_TRACE_FUNCTION;

	if (kConnected == _state || kDisconnecting == _state) {
		setState(kDisconnected);
		THEFOX_LOG(DEBUG) << "connection closed, addr:" << _peerAddr.toIpPort();

		_loop->delConnection(this);
		Socket::close(_socket.fd());

		if (_connectionCallback)
			_connectionCallback(this);
	}

	if (0 == _event.refCount()) {
		if (_removeConnectionCallback)
			_removeConnectionCallback(this); // 这是最后一行
		return;
	}
}
