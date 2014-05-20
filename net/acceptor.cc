#include <net/tcp_server.h>
#include <net/event_loop.h>
#include <net/inet_address.h>
#include <net/acceptor.h>
#include <net/tcp_connection.h>

using namespace thefox;

TcpServer::TcpServer(EventLoop *eventloop, const std::string &nameArg)
	: _eventloop(eventloop)
	, _name(nameArg)
	, _acceptor(new Acceptor())
{

}

TcpServer::~TcpServer()
{

    delete _acceptor();
}

bool TcpServer::start(const InetAddress &listenAddr)
{
    return _acceptor->listen(listenAddr);
}

bool TcpServer::started()
{
    return _model->started();
}

void TcpServer::send(const TcpConnectionPtr &conn, const char *data, size_t len)
{
    _model->send(conn, data, len);
}
void TcpServer::send(const TcpConnectionPtr &conn, const std::string &data)
{
    _model->send(conn, data.c_str(), data.length());
}

void TcpServer::removeConnection(TcpConnectionPtr conn)
{
    _model->removeConnection(conn);
}

void TcpServer::setConnectionCallback(const ConnectionCallback &cb)
{
    _model->setConnectionCallback(cb);
}
void TcpServer::setCloseCallback(const CloseCallback &cb)
{
    _model->setCloseCallback(cb);
}

void TcpServer::setMessageCallback(const MessageCallback &cb)
{
    _model->setMessageCallback(cb);
}

void TcpServer::setWriteCompleteCallback(const WriteCompleteCallback &cb)
{
    _model->setWriteCompleteCallback(cb);
}