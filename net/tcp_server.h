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
#include <net/tcp_handler.h>
#include <net/acceptor.h>

namespace thefox
{

class EventLoop;
class InetAddress;
class TcpConnection;

class TcpServer : public TcpHandler
{
public:
	friend class Acceptor;

    TcpServer(EventLoop *loop, const InetAddress &listenAddr, const std::string &nameArg);
    ~TcpServer(void);

    /// @brief 启动服务
    /// @return 成功返回true，否则返回false
    bool start();
    
    /// @brief 查看服务是否已经启动
    /// @return 已经启动返回true，否则返回false
	bool started() const { return _started; }

    /// @brief 移除客户连接
    void delConnection(TcpConnection *conn);
	
private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(TcpServer);
	void handleNewConnection(SOCKET sockfd, const InetAddress &peerAddr);

	typedef std::map<int32_t, TcpConnection *> ConnectionMap;
	typedef std::list<TcpConnection *> ConnectionList;

	const std::string _name;
	EventLoop *_loop; 
	Acceptor *_acceptor;

	bool _started;
    AtomicInt32 _nextConnId;
    Mutex _mutex;
	ConnectionMap _connections;
	ConnectionList _freeConnections;
};

} // namespace thefox

#endif // _THEFOX_NET_TCPSERVER_H_