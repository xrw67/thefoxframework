#ifndef _THEFOX_NET_CALLBACKS_H_
#define _THEFOX_NET_CALLBACKS_H_

#include <base/Timestamp.h>
#include <base/Types.h>

namespace thefox
{
    
class Buffer;
class TcpConnection;
typedef TcpConnection *TcpConnectionPtr;
typedef void (*ConnectionCallback)(const TcpConnectionPtr &conn);
typedef void (*CloseCallback)(const TcpConnectionPtr &conn);
typedef void (*MessageCallback)(const TcpConnectionPtr &conn,
                                Buffer *buffer,
                                Timestamp recvTime);
typedef void (*WriteCompleteCallback)(const TcpConnectionPtr &conn);

void defaultConnectionCallback(const TcpConnectionPtr &conn);
void defaultMessageCallback(const TcpConnectionPtr &conn,
                            Buffer *buffer,
                            Timestamp recvTime);

} // namespace thefox

#endif // _THEFOX_NET_CALLBACKS_H_