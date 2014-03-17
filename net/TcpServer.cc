#include <net/TcpServer.h>
#include <base/Types.h>

#ifdef WIN32
#include <net/IocpServer.h>
#else
#include <net/EpollServer.h>
#endif

using namespace thefox;

TcpServer::TcpServer(const String &nameArg, const InetAddress &listenAddr)
{
#ifdef WIN32
    _model = new IocpServer(nameArg, listenAddr);
#else
    _model = new EpollServer(nameArg, listenAddr);
#endif
}

TcpServer::~TcpServer()
{
	delete _model;
}

bool TcpServer::start()
{
    return _model->start();
}

bool TcpServer::started()
{
    return _model->started();
}

void TcpServer::send(const TcpConnectionPtr &conn, const char *data, size_t len)
{
	_model->send(conn, data, len);
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