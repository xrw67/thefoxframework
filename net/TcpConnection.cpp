#include <net/TcpConnection.h>
#include <base/Types.h>
#include <net/Event.h>
#include <net/Socket.h>
#include <net/Iocp.h>

using namespace thefox;
using namespace thefox::net;

TcpConnection::TcpConnection(SOCKET s, 
                             const String &name, 
                             const InetAddress &localAddr,
                             const InetAddress &peerAddr)
    : _socket(new Socket(s))
    , _name(name)
    , _localAddr(localAddr)
    , _peerAddr(peerAddr)
{
}

TcpConnection::~TcpConnection(void)
{

}

void TcpConnection::connectEstablished()
{
    
}

void TcpConnection::handleRead()
{

}

void TcpConnection::handleReadComplete()
{

}

void TcpConnection::handleWrite()
{

}

void TcpConnection::handleWriteComplete()
{

}

void TcpConnection::handleZeroByteRead()
{

}

void TcpConnection::handleZeroByteReadComplete()
{

}

void TcpConnection::handleClose()
{

}

Event *TcpConnection::getEvent()
{
    return NULL;
}

