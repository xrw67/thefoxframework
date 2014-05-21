#ifndef _THEFOX_NET_CALLBACKS_H_
#define _THEFOX_NET_CALLBACKS_H_

#include <base/types.h>
#include <base/timestamp.h>

namespace thefox
{
   
class Buffer;
class TcpConnection;

typedef std::function<void(const TcpConnection *conn)> ConnectionCallback;
typedef std::function<void(const TcpConnection *conn)> CloseCallback;
typedef std::function<void(const TcpConnection *conn, Buffer *buffer, const Timestamp recvTime)> MessageCallback;
typedef std::function<void(const TcpConnection *conn)> WriteCompleteCallback;

void defaultConnectionCallback(const TcpConnection *conn);
void defaultMessageCallback(const TcpConnection *conn, Buffer *buffer, const Timestamp recvTime);

} // namespace thefox

#endif // _THEFOX_NET_CALLBACKS_H_