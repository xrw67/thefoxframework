/*
 * @filename tcp_client.h
 * @brief tcp客户端连接类
 * @author macwe@qq.com
 */

#ifndef _THEFOX_NET_TCPCLIENT_H_
#define _THEFOX_NET_TCPCLIENT_H_

#include <base/common.h>
#include <net/callbacks.h>
#include <net/socket.h>

namespace thefox
{

class EventLoop;
class InetAddress;
class TcpConnection;

class TcpClient
{
public:
    TcpClient(EventLoop *loop, const std::string &nameArg);
    TcpClient(void);
        
    /// @brief 连接服务器
    /// @return 成功返回true，否则返回false
    bool open(const InetAddress &serverAddr);
        
    /// @brief 关闭连接
    void close();
        
    /// @brief 查看是否已经连接
    /// @return 已经连接返回true，否则返回false
    bool opened();
    
    /// @brief 发送数据
    void send(const char *data, size_t len);
    void send(const std::string &data);

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
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(TcpClient);

	void handleNewConnection(SOCKET sockfd, const InetAddress &peerAddr);
	void removeConnection(TcpConnection *conn);

	const std::string _name;
	EventLoop *_loop;
	TcpConnection *_conn;

	ConnectionCallback _connectionCallback;
    MessageCallback _messageCallback;
    WriteCompleteCallback _writeCompleteCallback;
};
    
} // namespace thefox

#endif // _THEFOX_NET_TCPCLIENT_H_