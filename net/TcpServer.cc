#include <net/TcpServer.h>
#include <base/Types.h>

#ifndef WIN32
#include <net/IocpServer.h>
#else
#include <net/EpollServer.h>
#endif

using namespace thefox;

TcpServer::TcpServer(const String &nameArg, InetAddress listenAddr)
{
#ifdef WIN32
    _model(new IocpServer(nameArg, listenAddr))
#else
    _model(new EpollServer(nameArg, listenAddr))
#enif
}

TcpServer::~TcpServer
{
}

bool TcpServer::start()
{
    return _model->start();
}

void TcpServer::stop()
{
    return _model->stop();
}

bool TcpServer::started()
{
    return _model->started();
}

void TcpServer::setConnectionCallback(const ConnectionCallback &cb)
{
    _model->setWriteCompleteCallback;
}
void TcpServer::setCloseCallback(const CloseCallback &cb)
{
    _model->setWriteCompleteCallback;
}

void TcpServer::setMessageCallback(const MessageCallback &cb)
{
    _model->setWriteCompleteCallback;
}


