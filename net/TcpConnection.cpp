#include <net/TcpConnection.h>
#include <base/Types.h>
#include <net/Socket.h>
#include <net/Iocp.h>

using namespace thefox;
using namespace thefox::net;

TcpConnection::TcpConnection(Iocp *iocp,
                             SOCKET s, 
                             const String &name, 
                             const InetAddress &localAddr,
                             const InetAddress &peerAddr)
    : _iocp(iocp)
    , _state(kConnecting)
    , _socket(new Socket(s))
    , _name(name)
    , _localAddr(localAddr)
    , _peerAddr(peerAddr)
    , _numberOfPendlingIO(0)
{
    _iocp->registerSocket(s, this);
}

TcpConnection::~TcpConnection(void)
{
    setState(kDisconnected);
}

void TcpConnection::send(const char *data, size_t len)
{
    _sendBuffer.append(data, len);
}

void TcpConnection::send(const String &data)
{
    send(data.c_str(), data.size());
}

void TcpConnection::connectEstablished()
{
    setState(kConnected);
    asyncRead();
    sendZeroByteRead();
    _connectionCallback(this);
}

void TcpConnection::handleClose()
{
    setState(kDisconnected);
    _closeCallback(this);
}

void TcpConnection::handleRead(IoContext *io)
{
    

}

void TcpConnection::handleReadComplete(IoContext *io)
{


    asyncRead();
    _messageCallback(this, &_readBuffer, Timestamp(Timestamp::now()));
}

void TcpConnection::handleWrite(IoContext *io)
{

}

void TcpConnection::handleWriteComplete(IoContext *io)
{

}

void TcpConnection::handleZeroByteRead(IoContext *io)
{

}

void TcpConnection::handleZeroByteReadComplete(IoContext *io)
{

}

void TcpConnection::asyncRead(IoContext *io)
{
    if (!io)
        io = IoContextPool::Instance().get();
    io->setIoType(IoContext::kRead);
    io->resetBuffer();
    _iocp->postCompletion(io, this, 0);
}


void TcpConnection::asyncWrite(IoContext *io)
{
    if (!io)
        io = IoContextPool::Instance().get();
}

void TcpConnection::sendZeroByteRead(IoContext *io)
{
    if (!io)
        io = IoContextPool::Instance().get();
}

void TcpConnection::handleEvent(IoContext *io)
{
    switch (io->getIoType()) {
    case IoContext::kRead:
        handleRead(io);
        break;
    case IoContext::kReadComplete:
        handleReadComplete(io);
        break;
    case IoContext::kWrite:
        handleWrite(io);
        break;
    case IoContext::kWriteComplete:
        handleWriteComplete(io);
        break;
    case IoContext::kZeroByteRead:
        handleZeroByteRead(io);
        break;
    case IoContext::kZeroByteReadComplete:
        handleZeroByteReadComplete(io);
        break;
    }
}