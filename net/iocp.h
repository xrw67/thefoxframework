/*
* @filename Iocp.h
* @brief 基于完成端口的socket实现
* @author macwe@qq.com
*/

#ifndef _THEFOX_NET_IOCP_H_
#define _THEFOX_NET_IOCP_H_

#include <map>

#include <base/types.h>
#include <base/mutex.h>
#include <base/thread.h>
#include <base/atomic_integer.h>
#include <net/inet_address.h>
#include <net/callbacks.h>

namespace thefox
{

class IoEvent;
class CpEvent;
class EventLoop;


class Iocp
{
public:
    friend class TcpConnection;
    
    Iocp(EventLoop *eventloop, const std::string &nameArg);
    ~Iocp();
    // 服务端方法
    bool start(const InetAddress &listenAddr);
    bool started() { return _started; }
    void send(const TcpConnectionPtr &conn, const char *data, size_t len);
    void removeConnection(TcpConnectionPtr conn);

    // 客户端方法
    bool open(const InetAddress &serverAddr);
    void close();
    bool isOpen();
    void send(const char *data, size_t len);

    // 接受者循环
    void acceptorLoop();
    

    void handleConnection(const TcpConnectionPtr &conn)
    { if (NULL != _connectionCallback) _connectionCallback(conn); }
    void handleClose(const TcpConnectionPtr &conn)
    { if (NULL != _closeCallback) _closeCallback(conn); }
    void handleMessage(const TcpConnectionPtr &conn, Buffer *buffer, Timestamp recvTime)
    { if (NULL != _messageCallback) _messageCallback(conn, buffer, recvTime); }
    void handleWriteComplete(const TcpConnectionPtr &conn)
    { if (NULL != _writeCompleteCallback) _writeCompleteCallback(conn); }

private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(Iocp);
	void postReadEvent(const TcpConnectionPtr &conn, CpEvent *e = NULL);
    void postWriteEvent(const TcpConnectionPtr &conn, CpEvent *e = NULL);
    void postZeroByteReadEvent(const TcpConnectionPtr &conn, CpEvent *e = NULL);
    void postCloseEvent(const TcpConnectionPtr &conn);

	// cp事件回调函数
	void handleCpError(IoEvent *evt);
	void handleCpRead(IoEvent *evt);
	void handleCpWrite(IoEvent *evt);
	void handleCpZeroByteRead(IoEvent *evt);
};

} // namespace thefox

#endif // _THEFOX_NET_IOCP_H_