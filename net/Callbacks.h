#ifndef _THEFOX_NET_CALLBACKS_H_
#define _THEFOX_NET_CALLBACKS_H_

#include <net/winapi.h>

class TcpConnection;
namespace thefox
{
    typedef TcpConnection *TcpConnectionPtr;
    typedef void (*ConnectionCallback)(const TcpConnectionPtr &);
    typedef void (*CloseCallback)(const TcpConnectionPtr &);
    typedef void (*WriteCompleteCallback)(const TcpConnectionPtr &);
    typedef void (*MessageCallback)(const TcpConnectionPtr &, Buffer *, Timestamp);
    typedef void (*ConnectionCallback)(const TcpConnectionPtr &);
    
    void defaultConnectionCallback(const TcpConnectionPtr &conn);
    void defaultMessageCallback(const TcpConnectionPtr &conn, Buffer* buffer,Timestamp receiveTime);
    
}

#endif // _THEFOX_NET_CALLBACKS_H_