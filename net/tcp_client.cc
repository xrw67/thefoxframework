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
    if (started()) {
        // LOG_WARN << tcpserver already started;
        return false;
    }
    _started = true;

    _socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (INVALID_SOCKET  == _socket) {
        int errCode = WSAGetLastError();
        // LOG_ERROR << WSASocket failed!, errcode:<< errCode;
        _started = false;
        return false;
    }

    int ret = connect(_socket, 
                      (struct sockaddr *)&serverAddr.getSockAddrInet(), 
                      sizeof(struct sockaddr_in));
    if (SOCKET_ERROR == ret && WSAEWOULDBLOCK != WSAGetLastError()) {
        // LOG_ERROR 
        closesocket(_socket);
        _started = false;
        return false;
    }

    newConnection(_socket, serverAddr);
    return true;
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
