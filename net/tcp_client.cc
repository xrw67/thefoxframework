#include <net/tcp_client.h>
#include <base/types.h>

#ifdef WIN32
#include <net/iocp.h>
#else
#include <net/epoll.h>
#endif

using namespace thefox;

TcpClient::TcpClient(EventLoop *eventloop, const std::string &nameArg)
{
#ifdef WIN32
    _model = new Iocp(eventloop, nameArg);
#else
    _model = new Epoll(eventloop, nameArg);
#endif
}

TcpClient::TcpClient()
{
    delete _model;
}

bool TcpClient::open(const InetAddress &serverAddr)
{
    return _model->open(serverAddr);
}

void TcpClient::close()
{
    _model->close();
}

bool TcpClient::isOpen()
{
    return _model->isOpen();
}

void TcpClient::send(const char *data, size_t len)
{
    _model->send(data, len);
}

void TcpClient::send(const std::string &data)
{
    _model->send(data.c_str(), data.length());
}
void TcpClient::setConnectionCallback(const ConnectionCallback &cb)
{
    _model->setConnectionCallback(cb);
}

void TcpClient::setCloseCallback(const CloseCallback &cb)
{
    _model->setCloseCallback(cb);
}

void TcpClient::setMessageCallback(const MessageCallback &cb)
{
    _model->setMessageCallback(cb);
}

void TcpClient::setWriteCompleteCallback(const WriteCompleteCallback &cb)
{
    _model->setWriteCompleteCallback(cb);
}
