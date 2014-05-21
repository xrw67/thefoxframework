#include <net/tcp_server.h>
#include <log/logging.h>
#include <net/event_loop.h>
#include <net/inet_address.h>
#include <net/tcp_connection.h>

using namespace thefox;

TcpServer::TcpServer(EventLoop *loop, const InetAddress &listenAddr, const std::string &nameArg)
	: _loop(loop)
	, _name(nameArg)
{
	_acceptor = new Acceptor(this, listenAddr);
}

TcpServer::~TcpServer()
{

    delete _acceptor;
}

bool TcpServer::start()
{
	if (_started)
		return true;

	if (_acceptor->listen()) {
		_started = true;
		THEFOX_LOG(INFO) << "TcpServer::start() done";
		return true;
	} else {
		THEFOX_LOG(ERROR) << "TcpServer::start() failed";
		return false;
	}
}

void TcpServer::delConnection(TcpConnection *conn)
{
}

void TcpServer::handleNewConnection(SOCKET sockfd, const InetAddress &peerAddr)
{
	int32_t id = _nextConnId.inc();

    TcpConnection *conn = 
		new TcpConnection((TcpHandler *)this, _loop, socket, id, peerAddr);
    _connections[id] = conn;

    _loop->addEvent(sockfd);
    
    if (NULL != conn) {
        conn->setState(TcpConnection::kConnected);
        postZeroByteReadEvent(conn);
        postReadEvent(conn);
        handleConnection(conn);
    }
}