/*
* @filename Iocp.h
* @brief 基于完成端口的socket实现
* @author macwe@qq.com
*/

#ifndef _THEFOX_NET_IOCP_H_
#define _THEFOX_NET_IOCP_H_

#include <map>
#include <Winsock2.h>
#include <base/noncopyable.h>
#include <base/Types.h>
#include <base/MutexLock.h>
#include <net/InetAddress.h>
#include <net/Callbacks.h>

namespace thefox
{

class CpEvent;
class EventLoop;
typedef std::map<int32_t, TcpConnectionPtr> ConnectionMap;

class Iocp : noncopyable
{
public:
    Iocp(EventLoop *eventloop, const String &nameArg);
    ~Iocp();
    // 服务端方法
    bool start(const InetAddress &listenAddr);
    bool started() { return _started; }
    void send(const TcpConnectionPtr &conn, const char *data, size_t len);
    void removeConnection(TcpConnectionPtr conn);

    // 客户端方法
    bool open(const InetAddress &serverAddr);
    void close();
    bool isOpen() { return _started; }
    void send(const char *data, size_t len);

    // 接受者循环
    void acceptorLoop();
    // 回调函数
    void setConnectionCallback(const ConnectionCallback &cb)
    { _connectionCallback = cb; }
    void setCloseCallback(const CloseCallback &cb)
    { _closeCallback = cb; }
    void setMessageCallback(const MessageCallback &cb)
    { _messageCallback = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback &cb)
    { _writeCompleteCallback = cb; }

    void handleConnection(const TcpConnectionPtr &conn)
    { if (NULL != _connectionCallback) _connectionCallback(conn); }
    void handleClose(const TcpConnectionPtr &conn)
    { if (NULL != _closeCallback) _closeCallback(conn); }
    void handleMessage(const TcpConnectionPtr &conn, Buffer *buffer, Timestamp recvTime)
    { if (NULL != _messageCallback) _messageCallback(conn, buffer, recvTime); }
    void handleWriteComplete(const TcpConnectionPtr &conn)
    { if (NULL != _writeCompleteCallback) _writeCompleteCallback(conn); }

    void postReadEvent(const TcpConnectionPtr &conn, CpEvent *e = NULL);
    void postWriteEvent(const TcpConnectionPtr &conn, CpEvent *e = NULL);
    void postZeroByteReadEvent(const TcpConnectionPtr &conn, CpEvent *e = NULL);
    void postCloseEvent(const TcpConnectionPtr &conn);

private:
    void newConnection(SOCKET socket, const InetAddress &peerAddr);

    ConnectionCallback _connectionCallback;
    CloseCallback _closeCallback;
    MessageCallback _messageCallback;
    WriteCompleteCallback _writeCompleteCallback;

    EventLoop *_eventloop;
    const String _name;
    SOCKET _socket;
    HANDLE _hAcceptEvent;
    bool _started;
    int32_t _nextConnId;

    ConnectionMap _connections;
    MutexLock _connMutex;
};

} // namespace thefox

#endif // _THEFOX_NET_IOCP_H_