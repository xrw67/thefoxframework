#ifndef _THEFOX_NET_CALLBACKS_H_
#define _THEFOX_NET_CALLBACKS_H_

#include <base/timestamp.h>

namespace thefox {
namespace net_asio {

class Buffer;
class TcpConnection;
typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

typedef std::function<void(const TcpConnectionPtr &conn)> ConnectionCallback;
typedef std::function<void(const TcpConnectionPtr &conn,
				Buffer *buffer, Timestamp recvTime)> MessageCallback;
typedef std::function<void(const TcpConnectionPtr &conn)> WriteCompleteCallback;

void defaultMessageCallback(const TcpConnectionPtr &conn,
						Buffer *buffer, Timestamp recvTime);

} // namespace net_asio
} // namespace thefox

#endif // _THEFOX_NET_CALLBACKS_H_