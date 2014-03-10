#include <net/TcpConnection.h>
#include <base/Types.h>
#include <net/Event.h>
#include <net/Socket.h>
#include <net/IoBufPool.h>

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

void TcpConnection::handleWrite()
{

}

void TcpConnection::handleClose()
{

}

Event *TcpConnection::getEvent()
{
    return NULL;
}

