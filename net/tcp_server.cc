#include <net/tcp_server.h>
#include <log/logging.h>
#include <net/tcp_connection.h>

using namespace thefox;

TcpServer::TcpServer(EventLoop *loop, const InetAddress &listenAddr, const std::string &nameArg)
	: _loop(loop)
	, _name(nameArg)
	, _started(false)
    , _messageCallback(defaultMessageCallback)
    , _acceptor(new Acceptor(listenAddr))
{
    _acceptor->init();
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

    _acceptor->setNewConnectionCallback(
        std::bind(&TcpServer::onNewConnection, this, _1, _2, _3));

	if (_acceptor->listen()) {
		_started = true;
		THEFOX_LOG(INFO) << "TcpServer::start() done";
	} else {
		THEFOX_LOG(ERROR) << "TcpServer::start() failed";
	}
	return _started;
}

void TcpServer::onNewConnection(SOCKET sockfd, const InetAddress &localAddr, const InetAddress &peerAddr)
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
	conn->setRemoveConnectionCallback(std::bind(&TcpServer::onRemoveConnection, this, _1));

	conn->connectEstablished();
}

void TcpServer::onRemoveConnection(TcpConnection *conn)
{
	THEFOX_TRACE_FUNCTION << "id=" << conn->id() << "addr=" << conn->peerAddr().toIpPort();

    assert(0 == conn->pendingIo());

	_connections.erase(conn->id());

	THEFOX_LOG(INFO) << "removeConnection done id=" << conn->id() << "addr=" << conn->peerAddr().toIpPort();

	_connectionPool.put(conn); // 析构,这是最后一行
	return;
}
