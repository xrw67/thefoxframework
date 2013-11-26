#ifndef _THEFOX_NET_CALLBACKS_H_
#define _THEFOX_NET_CALLBACKS_H_

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

namespace thefox
{
	
class Buffer;
class TcpConnection;

typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;

typedef boost::function<void()> TimerCallback;
typedef boost::function<void (const TcpConnectionPtr &)> ConnectionCallback;
typedef boost::function<void (const TcpConnectionPtr &)> CloseCallback;
typedef boost::function<void (const TcpConnectionPtr &)> WriteCompleteCallback;
typedef boost::function<void (const TcpConnectionPtr&, Buffer*, Timestamp)> MessageCallback;

void defaultConnectionCallback(const TcpConnectionPtr& conn);
void defaultMessageCallback(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp receiveTime);

}

#endif // _THEFOX_NET_CALLBACKS_H_