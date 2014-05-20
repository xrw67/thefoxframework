/*
 * @filename TcpServer.h
 * @brief tcp服务器类
 * @author macwe@qq.com
 */

#ifndef _THEFOX_NET_TCPSERVER_H_
#define _THEFOX_NET_TCPSERVER_H_

#include <map>
#include <list>
#include <base/types.h>
#include <base/mutex.h>
#include <base/Atomic_integer.h>
#include <net/callbacks.h>

namespace thefox
{

class EventLoop;
class InetAddress;
class Acceptor;
class TcpConnection;

class TcpServer
{
public:
    TcpServer(EventLoop *eventloop, const std::string &nameArg);
    ~TcpServer(void);

    /// @brief 启动服务
    /// @return 成功返回true，否则返回false
    bool start(const InetAddress &listenAddr);
    
    /// @brief 查看服务是否已经启动
    /// @return 已经启动返回true，否则返回false
    bool started();

    /// @brief 移除客户连接
    void removeConnection(TcpConnection *conn);

    /// @brief 设置连接状态改变回调函数
    void setConnectionCallback(const ConnectionCallback &cb);
    
    /// @brief 设置连接关闭回调函数
    void setCloseCallback(const CloseCallback &cb);
    
    /// @brief 设置收到数据的回调函数
    void setMessageCallback(const MessageCallback &cb);

    /// @brief 设置缓冲区中数据发送完成后的回调函数
    void setWriteCompleteCallback(const WriteCompleteCallback &cb);

private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(TcpServer);
	typedef std::map<int32_t, TcpConnection *> ConnectionMap;
	typedef std::list<TcpConnection *> ConnectionList;

	EventLoop *_eventloop; 
    const std::string _name;
	Acceptor *_acceptor;


	bool _started;
    AtomicInt32 _nextConnId;
    Mutex _mutex;
	ConnectionMap _connections;
	ConnectionList _freeConnections;

    ConnectionCallback _connectionCallback;
    CloseCallback _closeCallback;
    MessageCallback _messageCallback;
    WriteCompleteCallback _writeCompleteCallback;
};

} // namespace thefox

#endif // _THEFOX_NET_TCPSERVER_H_