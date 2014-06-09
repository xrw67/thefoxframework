#include <net/tcp_connection.h>
#include <log/logging.h>

using namespace thefox;
using namespace thefox::net;

void thefox::net::defaultMessageCallback(TcpConnection *conn,
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
    , _pendingEvents(0)
{
	THEFOX_TRACE_FUNCTION;
}

TcpConnection::~TcpConnection()
{
	THEFOX_TRACE_FUNCTION;
}

void TcpConnection::send(const string &data)
{
	this->send(data.c_str(), data.length());
}

void TcpConnection::send(const char *data, size_t len)
{
	_writeBuffer.append(data, len);

    if (!_event.testAndSetWrite()) {
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
        connectDestroyed();
	}
}

void TcpConnection::connectEstablished()
{
	THEFOX_TRACE_FUNCTION;

	assert(kConnecting == _state );

	setState(kConnected);

    _event.fd = fd();
    _event.conn = this;

    _loop->addEvent(&_event);

	// set read
	_loop->updateRead(&_event);

    if (_connectionCallback) {
        _connectionCallback(this);
    }
}

void TcpConnection::connectDestroyed()
{
	THEFOX_TRACE_FUNCTION;

    _event.error = true;

	if (kConnected == _state || kDisconnecting == _state) {
		setState(kDisconnected);
		THEFOX_LOG(DEBUG) << "connection closed, addr:" << _peerAddr.toIpPort();

        _loop->delEvent(&_event);
		Socket::close(_socket.fd());

        if (_connectionCallback) {
            _connectionCallback(this);
        }
	}

    if (0 == leaveEvent()) {
        if (_removeConnectionCallback) {
            _removeConnectionCallback(this); // it's last line, connection is over
            return;
        }
    }
}

void TcpConnection::enterEvent()
{
    MutexGuard lock(_eventmutex);
    ++_pendingEvents;
}

size_t TcpConnection::leaveEvent()
{
    MutexGuard lock(_eventmutex);
    if (0 != _pendingEvents) {
       --_pendingEvents;
    }
    return _pendingEvents;
}
