#include <net/tcp_handler.h>
#include <log/logging.h>
#include <net/buffer.h>
#include <net/tcp_connection.h>

namespace thefox
{

void defaultConnectionCallback(TcpConnection *conn)
{
	THEFOX_LOG(INFO) 
		<< "new connection coming! addr:" << conn->peerAddr().toIpPort();

    return;
}

void defaultMessageCallback(TcpConnection *conn, Buffer *buffer, Timestamp recvTime)
{
    buffer->retrieveAll();
}

} // namespace thefox

using namespace thefox;

TcpHandler::TcpHandler()
	: _connectionCallback(std::bind(defaultConnectionCallback))
	, _messageCallback(std::bind(defaultMessageCallback))
	, _writeCompleteCallback(NULL)
	, _closeCallback(NULL)
{
}

TcpHandler::~TcpHandler()
{
}