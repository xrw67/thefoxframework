#ifndef _THEFOX_NET_CALLBACKS_H_
#define _THEFOX_NET_CALLBACKS_H_

#include <base/Types.h>
#include <base/Timestamp.h>

namespace thefox
{
    
class Buffer;
class Connection;
typedef std::shared_ptr<Connection> ConnectionPtr;

typedef std::function<void(const ConnectionPtr &conn)> ConnectionCallback;
typedef std::function<void(const ConnectionPtr &conn)> CloseCallback;
typedef std::function<void(const ConnectionPtr &conn, Buffer *buffer, const Timestamp recvTime)> MessageCallback;
typedef std::function<void(const ConnectionPtr &conn)> WriteCompleteCallback;

void defaultConnectionCallback(const ConnectionPtr &conn);
void defaultMessageCallback(const ConnectionPtr &conn, Buffer *buffer, const Timestamp recvTime);

} // namespace thefox

#endif // _THEFOX_NET_CALLBACKS_H_