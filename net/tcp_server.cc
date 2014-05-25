#include <net/tcp_server.h>
#include <log/logging.h>
#include <net/event_loop.h>
#include <net/inet_address.h>
#include <net/tcp_connection.h>

using namespace thefox;

TcpServer::TcpServer(EventLoop *loop, const InetAddress &listenAddr, const std::string &nameArg)
	: _loop(loop)
	, _name(nameArg)
	, _started(false)
	, _connectionCallback(NULL)
	, _messageCallback(defaultMessageCallback)
	, _writeCompleteCallback(NULL)
{
	THEFOX_TRACE_FUNCTION;

	_acceptor = new Acceptor(this, listenAddr);
}

TcpServer::~TcpServer()
{
	THEFOX_TRACE_FUNCTION;

	_started = false;
    delete _acceptor;
}

bool TcpServer::start()
{
	THEFOX_TRACE_FUNCTION;

	if (_started)
		return true;

	if (_acceptor->listen()) {
		_started = true;
		THEFOX_LOG(INFO) << "TcpServer::start() done";
	} else {
		THEFOX_LOG(ERROR) << "TcpServer::start() failed";
	}
	return _started;
}

void TcpServer::handleNewConnection(SOCKET sockfd, const InetAddress &localAddr, const InetAddress &peerAddr)
{
	THEFOX_TRACE_FUNCTION << "peerAddr:" << peerAddr.toIpPort();

	int32_t id = _nextConnId.inc();

    TcpConnection *conn = 
		_connectionPool.get<EventLoop *, SOCKET, int32_t, const InetAddress &, const InetAddress &>
							(_loop, sockfd, id, localAddr, peerAddr);
    _connections[id] = conn;

	conn->setConnectionCallback(_connectionCallback);
	conn->setMessageCallback(_messageCallback);
	conn->setWriteCompleteCallback(_writeCompleteCallback);
	conn->setRemoveConnectionCallback(std::bind(&TcpServer::removeConnection, this, _1));

	conn->connectEstablished();
}

void TcpServer::removeConnection(TcpConnection *conn)
{
	THEFOX_TRACE_FUNCTION << "id=" << conn->id() << "addr=" << conn->peerAddr().toIpPort();

	_connections.erase(conn->id());

	THEFOX_LOG(INFO) << "removeConnection done id=" << conn->id() << "addr=" << conn->peerAddr().toIpPort();

	_connectionPool.put(conn); // 析构,这是最后一行
	return;
}