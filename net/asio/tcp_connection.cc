#include <net/asio/tcp_connection.h>
#include <log/logging.h>

using namespace thefox;
using namespace thefox::net_asio;

void thefox::net_asio::defaultMessageCallback(const TcpConnectionPtr &conn,
							Buffer *buf, Timestamp recvTime)
{
	buf->retrieveAll();
}

TcpConnection::TcpConnection(io_service &io, int id)
	: _io(io)
    , _id(id)
    , _readBytes(0)
    , _writeBytes(0)
	, _arg(NULL)
	, _state(kConnecting)
	, _socket(io)
{
}

TcpConnection::~TcpConnection()
{
}

void TcpConnection::send(const string &data)
{
	this->send(data.c_str(), data.length());
}

void TcpConnection::send(const char *data, size_t len)
{
	bool writeInProgress = (_writeBuffer.readableBytes() > 0);
	_writeBuffer.append(data, len);

	_io.post(
		[this, writeInProgress]() {
		if (!writeInProgress) {
				doWrite();
			}
		});
}

void TcpConnection::shutdown()
{
	THEFOX_TRACE_FUNCTION;

	_socket.shutdown(ip::tcp::socket::shutdown_both);
}

void TcpConnection::setTcpNoDelay(bool on)
{
	THEFOX_TRACE_FUNCTION << "on=" << on;

	ip::tcp::no_delay option(on);
	_socket.set_option(option);
}

void TcpConnection::forceClose()
{
	THEFOX_TRACE_FUNCTION;

	if (kConnected == _state || kDisconnecting == _state) {
		setState(kDisconnecting);
		_io.post([this]() { connectDestroyed(); });
	}
}

void TcpConnection::connectEstablished()
{
	THEFOX_TRACE_FUNCTION;

	assert(kConnecting == _state );

	setState(kConnected);

	auto self(shared_from_this());

    if (_connectionCallback) {
		_connectionCallback(self);
    }

	doRead();
}

void TcpConnection::connectDestroyed()
{
	THEFOX_TRACE_FUNCTION;

	auto self(shared_from_this());

	_socket.close();

	if (kConnected == _state || kDisconnecting == _state) {
		setState(kDisconnected);
		THEFOX_LOG(DEBUG) << "connection closed";

        if (_connectionCallback) {
			_connectionCallback(self);
        }
	}

    if (_removeConnectionCallback) {
		_removeConnectionCallback(self); // it's last line, connection is over
    }
}

void TcpConnection::doRead()
{
	auto self(shared_from_this());

	_readBuffer.ensureWritableBytes(8192);
	_socket.async_read_some(boost::asio::buffer(_readBuffer.beginWrite(), _readBuffer.writableBytes()),
		[this, self](const boost::system::error_code &ec, size_t length){
			if (!ec) {
				_readBuffer.hasWritten(length);
				_readBytes += length;

				if (_messageCallback) {
					_messageCallback(self, &_readBuffer, Timestamp(Timestamp::now()));
				}
				doRead();
			} else {
				connectDestroyed();
			}
		});
}

void TcpConnection::doWrite()
{
	auto self(shared_from_this());
	async_write(_socket, 
		boost::asio::buffer(_writeBuffer.peek(), _writeBuffer.readableBytes()),
		[this, self](const boost::system::error_code &ec, size_t length) {
		if (!ec) {
			_writeBuffer.retrieve(length);
			_writeBytes += length;

			if (0 == _writeBuffer.readableBytes()) {
				if (_writeCompleteCallback) {
					_writeCompleteCallback(self);
				}
			} else {
				doWrite();
			}
		}
		else {
			connectDestroyed();
		}
	});
}