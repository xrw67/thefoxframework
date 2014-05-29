/*
 * @filename TcpServer.h
 * @brief tcp服务器类
 * @author macwe@qq.com
 */

#ifndef _THEFOX_NET_TCPSERVER_H_
#define _THEFOX_NET_TCPSERVER_H_

#include <map>
#include <list>
#include <base/common.h>
#include <base/mutex.h>
#include <base/Atomic_integer.h>
#include <base/object_pool.h>
#include <net/callbacks.h>
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

    /// @brief 启动服务
    /// @return 成功返回true，否则返回false
    bool start();
    
    /// @brief 查看服务是否已经启动
    /// @return 已经启动返回true，否则返回false
	bool started() const { return _started; }
	
	/// @brief 设置连接状态改变回调函数
    void setConnectionCallback(const ConnectionCallback &cb)
    { _connectionCallback = cb; }
	/// @brief 设置收到数据的回调函数
    void setMessageCallback(const MessageCallback &cb)
    { _messageCallback = cb; }
	/// @brief 设置缓冲区中数据发送完成后的回调函数
    void setWriteCompleteCallback(const WriteCompleteCallback &cb)
    { _writeCompleteCallback = cb; }

private:
	friend class Acceptor;
	friend class TcpConnection;

	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(TcpServer);

	void handleNewConnection(SOCKET sockfd, const InetAddress &localAddr, const InetAddress &peerAddr);
	void removeConnection(TcpConnection *conn);

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