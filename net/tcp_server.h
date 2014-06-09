/*
 * @brief tcp server
 * @author macwe1024 at gmail dot com
 */

#ifndef _THEFOX_NET_TCPSERVER_H_
#define _THEFOX_NET_TCPSERVER_H_

#include <map>
#include <list>
#include <base/common.h>
#include <base/mutex.h>
#include <base/atomic_integer.h>
#include <base/object_pool.h>
#include <net/callbacks.h>
#include <net/inet_address.h>
#include <net/acceptor.h>

namespace thefox
{

class EventLoop;
class InetAddress;
class TcpConnection;

class TcpServer
{
public:
    TcpServer(EventLoop *loop, const InetAddress &listenAddr, const std::string &nameArg);
    ~TcpServer(void);
	
    /// @brief start the server
    /// @return return true if success，then return false
    bool start();
    
    /// @brief check if the server started
    /// @return return true if success，then return false
	bool started() const { return _started; }

    void setConnectionCallback(const ConnectionCallback &cb)
    { 
        _connectionCallback = cb; 
    }
    void setMessageCallback(const MessageCallback &cb)
    {
        _messageCallback = cb;
    }
    void setWriteCompleteCallback(const WriteCompleteCallback &cb)
    {
        _writeCompleteCallback = cb; 
    }

private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(TcpServer);

	void onNewConnection(SOCKET sockfd, const InetAddress &localAddr, const InetAddress &peerAddr);
	void onRemoveConnection(TcpConnection *conn);

	typedef std::map<int32_t, TcpConnection *> ConnectionMap;

	const std::string _name;
	EventLoop *_loop; 
	Acceptor *_acceptor;

	bool _started;
    AtomicInt32 _nextConnId;
    Mutex _mutex;
	ConnectionMap _connections;
	ObjectPool<TcpConnection> _connectionPool;

	ConnectionCallback _connectionCallback;
    MessageCallback _messageCallback;
    WriteCompleteCallback _writeCompleteCallback;
};

} // namespace thefox

#endif // _THEFOX_NET_TCPSERVER_H_
