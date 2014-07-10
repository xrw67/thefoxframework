#include <net/asio/tcp_connection.h>
#include <log/logging.h>

using namespace thefox;
using namespace thefox::net_asio;

void thefox::defaultMessageCallback(const TcpConnectionPtr &conn,
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
	, _pendingIo(0)
{
}

TcpConnection::~TcpConnection()
{
	_arg = NULL;
}

void TcpConnection::send(const string &data)
{
	this->send(data.c_str(), data.length());
}

void TcpConnection::send(const char *data, size_t len)
{
	bool writeInProgress = (_writeBuffer.readableBytes() > 0);
	_writeBuffer.append(data, len);

	enterIo();
	_io.post([this, writeInProgress]() {
		if (!writeInProgress) {
			doWrite();
		}
		leaveIo();
	});
}

void TcpConnection::shutdown()
{
	_socket.shutdown(ip::tcp::socket::shutdown_both);
}

void TcpConnection::setTcpNoDelay(bool on)
{
	ip::tcp::no_delay option(on);
	_socket.set_option(option);
}

void TcpConnection::forceClose()
{
	if (kConnected == _state || kDisconnecting == _state) {
		setState(kDisconnecting);
		enterIo();
		_io.post([this]() { 
			connectDestroyed();
			leaveIo();
		});
	}
}

void TcpConnection::connectEstablished()
{
	assert(kConnecting == _state );

	setState(kConnected);

    if (_connectionCallback) {
		_connectionCallback(shared_from_this());
    }

	doRead();
}

void TcpConnection::connectDestroyed()
{
	if (kConnected == _state || kDisconnecting == _state) {
		setState(kDisconnected);
		_socket.close();
	}	
}

void TcpConnection::doRead()
{
	enterIo();

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
			leaveIo();
		});
}

void TcpConnection::doWrite()
{
	enterIo();

	auto self(shared_from_this());

	async_write(_socket, 
		boost::asio::buffer(_writeBuffer.peek(), _writeBuffer.readableBytes()),
		[this, self](const boost::system::error_code &ec, size_t length) {
		if (!ec) {
			_writeBuffer.retrieve(length);
			_writeBytes += length;

			if (0 == _writeBuffer.readableBytes() && _writeCompleteCallback) {
				_writeCompleteCallback(self);
			} 

			if (_writeBuffer.readableBytes() > 0) {
				doWrite();
			}
		}
		else {
			connectDestroyed();
		}
		leaveIo();
	});
}

void TcpConnection::enterIo()
{
	MutexGuard lock(_ioMutex);
	++_pendingIo;
}

void TcpConnection::leaveIo()
{
	{
	MutexGuard lock(_ioMutex);
	--_pendingIo;
	}

	assert(_pendingIo >= 0);

	if (0 == _pendingIo) {
		if (kDisconnected == _state ) {
			auto self(shared_from_this());
			if (_connectionCallback) {
				_connectionCallback(self);
			}
			if (_removeConnectionCallback) {
				_removeConnectionCallback(self); // it's last line, connection is over
			}
		}
	}
}
