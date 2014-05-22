#ifndef _THEFOX_NET_CALLBACKS_H_
#define _THEFOX_NET_CALLBACKS_H_

#include <base/types.h>
#include <base/timestamp.h>
#include <base/closure.h>

namespace thefox
{
   
class Buffer;
class TcpConnection;

typedef Closure<void(const TcpConnection *conn)> ConnectionClosure;
typedef Closure<void(const TcpConnection *conn)> CloseClosure;
typedef Closure<void(const TcpConnection *conn, Buffer *buffer, const Timestamp recvTime)> MessageClosure;
typedef Closure<void(const TcpConnection *conn)> WriteCompleteClosure;

} // namespace thefox

#endif // _THEFOX_NET_CALLBACKS_H_