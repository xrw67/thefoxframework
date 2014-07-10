#include <net/asio/tcp_client.h>
#include <net/asio/tcp_connection.h>
#include <log/logging.h>

using namespace thefox;
using namespace thefox::net_asio;

TcpClient::TcpClient(io_service &io, const string &nameArg)
	: _io(io)
	, _name(nameArg)
	, _messageCallback(defaultMessageCallback)
{
}

TcpClient::~TcpClient()
{
    close();
}

bool TcpClient::open(ip::tcp::endpoint endpoint)
{
    if (opened()) {
        return false;
    }

	_conn = std::make_shared<TcpConnection>(_io, 1);

	_conn->socket().async_connect(endpoint,
		[this](const boost::system::error_code &ec) {
		if (!ec) {
			_conn->setConnectionCallback(_connectionCallback);
			_conn->setMessageCallback(_messageCallback);
			_conn->setWriteCompleteCallback(_writeCompleteCallback);
			_conn->setRemoveConnectionCallback(std::bind(&TcpClient::removeConnection, this, _1));
			_conn->connectEstablished();
		}
	});
    return true;
}

void TcpClient::close()
{
	if (_conn) {
		_conn->forceClose();
	}
}

bool TcpClient::opened()
{
	if (_conn) {
		if (TcpConnection::kConnected == _conn->state()) {
			return true;
		}
	}
	
	return false;
}

void TcpClient::send(const char *data, size_t len)
{
	if (_conn) {
		_conn->send(data, len);
	}
}

void TcpClient::send(const string &data)
{
	if (_conn) {
		_conn->send(data);
	}
}

void TcpClient::removeConnection(const TcpConnectionPtr &conn)
{
	_conn.reset();
}
