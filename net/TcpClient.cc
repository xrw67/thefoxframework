#include <net/TcpClient.h>
#include <base/Types.h>

#ifdef WIN32
#include <net/Iocp.h>
#else
#include <net/Epoll.h>
#endif

using namespace thefox;

TcpClient::TcpClient(const String &nameArg)
{
#ifdef WIN32
    _model = new Iocp(nameArg);
#else
    _model = new Epoll(nameArg);
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

void TcpClient::send(const String &data)
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
