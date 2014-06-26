#include <net/asio/tcp_client.h>
#include <net/asio/tcp_connection.h>
#include <log/logging.h>

using namespace thefox;
using namespace thefox::net_asio;

TcpClient::TcpClient(io_service &io, const string &nameArg)
	: _io(io)
	, _name(nameArg)
	, _conn(new TcpConnection(io, 1))
	, _messageCallback(defaultMessageCallback)
{
}

TcpClient::~TcpClient()
{
    close();
}

bool TcpClient::open(ip::tcp::endpoint endpoint)
{
	THEFOX_TRACE_FUNCTION;
    if (opened()) {
        return false;
    }

	_conn->socket().async_connect(endpoint,
		[this](const boost::system::error_code &ec) {
		if (!ec) {
			_conn->setConnectionCallback(_connectionCallback);
			_conn->setMessageCallback(_messageCallback);
			_conn->setWriteCompleteCallback(_writeCompleteCallback);
			_conn->connectEstablished();
		}
	});
    return true;
}

void TcpClient::close()
{
	_io.post([this](){ _conn->forceClose(); });
}

bool TcpClient::opened()
{
	if (TcpConnection::kConnected == _conn->state()) {
			return true;
	}
	return false;
}

void TcpClient::send(const char *data, size_t len)
{
	_conn->send(data, len);
}

void TcpClient::send(const string &data)
{
	_conn->send(data);
}