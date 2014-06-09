#ifndef _THEFOX_NET_CALLBACKS_H_
#define _THEFOX_NET_CALLBACKS_H_

#include <base/common.h>
#include <base/timestamp.h>

namespace thefox {
namespace net {
   
class Buffer;
class TcpConnection;

typedef std::function<void(TcpConnection *conn)> ConnectionCallback;
typedef std::function<void(TcpConnection *conn, 
                           Buffer *buffer, const Timestamp recvTime)> MessageCallback;
typedef std::function<void(TcpConnection *conn)> WriteCompleteCallback;

void defaultMessageCallback(TcpConnection *conn, 
							Buffer *buffer, const Timestamp recvTime);

} // namespace net
} // namespace thefox

#endif // _THEFOX_NET_CALLBACKS_H_