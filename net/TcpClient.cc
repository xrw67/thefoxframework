#include <net/TcpClient.h>
#include <base/Types.h>

#ifndef WIN32
#include <net/WsaEventClient.h>
#else
#include <net/pollClient.h>
#endif

using namespace thefox;

TcpClient::TcpClient(InetAddress serverAddr)
{
#ifdef WIN32
    _model(new WsaEventClient(serverAddr))
#else
    _model(new PollClient(serverAddr))
#enif
}

TcpClient::TcpClient
{
}

bool TcpClient::open()
{
    return _model->open();
}

void TcpClient::close()
{
    return _model->close();
}

bool TcpClient::isOpen()
{
    return _model->isOpen();
}

void TcpClient::setConnectionCallback(const ConnectionCallback &cb)
{
    _model->setWriteCompleteCallback();
}
void TcpClient::setCloseCallback(const CloseCallback &cb)
{
    _model->setWriteCompleteCallback();
}

void TcpClient::setMessageCallback(const MessageCallback &cb)
{
    _model->setWriteCompleteCallback();
}


