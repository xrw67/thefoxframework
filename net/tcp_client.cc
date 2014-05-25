#include <net/tcp_client.h>
#include <log/logging.h>
#include <net/socket.h>
#include <net/tcp_connection.h>

using namespace thefox;

TcpClient::TcpClient(EventLoop *loop, const std::string &nameArg)
	: _loop(loop)
	, _name(nameArg)
	, _conn(NULL)
	, _connectionCallback(NULL)
	, _messageCallback(defaultMessageCallback)
	, _writeCompleteCallback(NULL)
{

}

TcpClient::TcpClient()
{
    close();
}

bool TcpClient::open(const InetAddress &serverAddr)
{
	THEFOX_TRACE_FUNCTION << "addr:" << serverAddr.toIpPort();
    if (opened()) {
        THEFOX_LOG(WARN) << "server:"<< serverAddr.toIpPort() << "opened";
        return false;
    }

    SOCKET sockfd = Socket::create();
    if (sockfd < 0) {
        return false;
    }

	if (!Socket::connect(sockfd, serverAddr)) {
		Socket::close(sockfd);
		return false;
	}

    handleNewConnection(sockfd, serverAddr);
    return true;
}

void TcpClient::close()
{
	if (_conn)
		_conn->forceClose();
}

bool TcpClient::opened()
{
	if (NULL != _conn) {
		if (TcpConnection::kConnected == _conn->state())
			return true;
	}
	return false;
}

void TcpClient::send(const char *data, size_t len)
{
	if (_conn)
		_conn->send(data, len);
}

void TcpClient::send(const std::string &data)
{
	if (_conn)
		_conn->send(data);
}

void TcpClient::handleNewConnection(SOCKET sockfd, const InetAddress &peerAddr)
{
	THEFOX_TRACE_FUNCTION;

	InetAddress localAddr(Socket::getLocalAddr(sockfd));
    _conn = new TcpConnection(_loop, sockfd, 0, localAddr, peerAddr);

	_conn->setConnectionCallback(_connectionCallback);
	_conn->setMessageCallback(_messageCallback);
	_conn->setWriteCompleteCallback(_writeCompleteCallback);
	_conn->setRemoveConnectionCallback(std::bind(&TcpClient::removeConnection, this, _1));

	_conn->connectEstablished();
}

void TcpClient::removeConnection(TcpConnection *conn)
{
	THEFOX_TRACE_FUNCTION;

	SAFE_DELETE(_conn);
}