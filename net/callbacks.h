#ifndef _THEFOX_NET_CALLBACKS_H_
#define _THEFOX_NET_CALLBACKS_H_

#include <base/types.h>
#include <base/timestamp.h>

namespace thefox
{
    
class Buffer;
class TcpConnection;
typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

typedef std::function<void(const TcpConnectionPtr &conn)> ConnectionCallback;
typedef std::function<void(const TcpConnectionPtr &conn)> CloseCallback;
typedef std::function<void(const TcpConnectionPtr &conn, Buffer *buffer, const Timestamp recvTime)> MessageCallback;
typedef std::function<void(const TcpConnectionPtr &conn)> WriteCompleteCallback;

void defaultConnectionCallback(const TcpConnectionPtr &conn);
void defaultMessageCallback(const TcpConnectionPtr &conn, Buffer *buffer, const Timestamp recvTime);

} // namespace thefox

#endif // _THEFOX_NET_CALLBACKS_H_