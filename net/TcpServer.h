/*
 * @filename TcpServer.h
 * @brief tcp服务器类
 * @author macwe@qq.com
 */

#ifndef _THEFOX_NET_TCPSERVER_H_
#define _THEFOX_NET_TCPSERVER_H_

#include <base/Types.h>
#include <base/noncopyable.h>
#include <net/Callbacks.h>

namespace thefox
{

#ifdef WIN32
    class IocpServer;
#else
    class EpollServer;
#endif

class InetAddress;
class TcpConnection;

class TcpServer : noncopyable
{
public:
	TcpServer(const String &nameArg, const InetAddress &listenAddr);
	~TcpServer(void);

    /// @brief 启动服务
    /// @return 成功返回true，否则返回false
    bool start();
    
    /// @brief 查看服务是否已经启动
    /// @return 已经启动返回true，否则返回false
    bool started();
    
	/// @brief 发送数据
	void send(const TcpConnectionPtr &conn, const char *data, size_t len);

	/// @brief 移除客户连接
	void removeConnection(const TcpConnectionPtr &conn);

    /// @brief 设置连接状态改变回调函数
    void setConnectionCallback(const ConnectionCallback &cb);
    
    /// @brief 设置连接关闭回调函数
    void setCloseCallback(const CloseCallback &cb);
    
    /// @brief 设置收到数据的回调函数
    void setMessageCallback(const MessageCallback &cb);

	/// @brief 设置缓冲区中数据发送完成后的回调函数
	void setWriteCompleteCallback(const WriteCompleteCallback &cb);
private:
#ifdef WIN32
    IocpServer *_model;
#else
    EpollServer *_model;
#endif
};

} // namespace thefox

#endif // _THEFOX_NET_TCPSERVER_H_