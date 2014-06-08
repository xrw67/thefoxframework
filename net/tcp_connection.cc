#include <net/tcp_connection.h>
#include <log/logging.h>
#include <net/event_loop.h>

using namespace thefox;

void thefox::defaultMessageCallback(TcpConnection *conn,
                                Buffer* buf, const Timestamp recvTime)
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
    , _pendingIo(0)
    , _write(false)
{
	THEFOX_TRACE_FUNCTION;
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

    if (!testAndSetWrite()) {
        _loop->updateWrite(this);
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
        connectDestroyed();
	}
}

void TcpConnection::connectEstablished()
{
	THEFOX_TRACE_FUNCTION;

	assert(kConnecting == _state );

	setState(kConnected);

    _loop->registerConnection(this);

	// 投递读事件
	_loop->updateRead(this);

    if (_connectionCallback) {
        _connectionCallback(this);
    }
}

void TcpConnection::connectDestroyed()
{
	THEFOX_TRACE_FUNCTION;

	if (kConnected == _state || kDisconnecting == _state) {
		setState(kDisconnected);
		THEFOX_LOG(DEBUG) << "connection closed, addr:" << _peerAddr.toIpPort();

        _loop->unregisterConnection(this);
		Socket::close(_socket.fd());

        if (_connectionCallback) {
            _connectionCallback(this);
        }
	}

    if (0 == pendingIo() && _removeConnectionCallback) {
        _removeConnectionCallback(this); // 这是最后一行
        return;
    }
}

bool TcpConnection::testAndSetWrite()
{
    MutexGuard lock(_writemutex);
    bool ret = _write;
    _write = true;
    return ret;
}

void TcpConnection::resetWrite()
{
    MutexGuard lock(_writemutex);
    _write = false;
}

void TcpConnection::enterIo()
{
    MutexGuard lock(_iomutex);
    ++_pendingIo;
}

size_t TcpConnection::leaveIo()
{
    MutexGuard lock(_iomutex);
    --_pendingIo;
    return _pendingIo;
}
