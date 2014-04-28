/*
* @filename TcpConnection.h
* @brief 表示一个客户连接
* @author macwe@qq.com
*/

#ifndef _THEFOX_NET_TCPCONNECTION_H_
#define _THEFOX_NET_TCPCONNECTION_H_

#include <base/Types.h>
#include <base/MutexLock.h>
#include <net/Buffer.h>
#include <net/InetAddress.h>

namespace thefox
{

#ifndef WIN32
typedef int SOCKET
#endif

typedef std::function<void()> PostEventFunction;

class TcpConnection
{
public:
    enum StateT { kDisconnected, kConnecting, kConnected, kDisconnecting };

    TcpConnection(SOCKET socket, int connId, const InetAddress &peerAddr)
        : _socket(socket)
        , _connId(connId)
        , _peerAddr(peerAddr)
        , _pendingEvent(0)
        , _readBytes(0)
        , _writeBytes(0)
    {}
    ~TcpConnection()
    {
        if (INVALID_SOCKET != _socket) {
            closesocket(_socket);
            _socket = INVALID_SOCKET;
        }
    }

    /// @brief 获取连接ID
    int connId() const { return _connId; }

    /// @brief 获取SOCKET句柄
    SOCKET socket() { return _socket; }

    /// @brief 获取对端ip地址
    const InetAddress &getPeerAddr() const { return _peerAddr; }

    /// @brief 获取Read缓冲区指针
    Buffer *readBuffer() { return &_readBuffer; }

    /// @brief 获取Write缓冲区指针
    Buffer *writeBuffer() { return &_writeBuffer; }

    /// @brief 获取连接状态
    StateT state() const { return _state; }

    /// @brief 
    void *context() const { return _context; }

    /// @brief 
    void setContext(void *context) { _context = context; }

    //以下为内部使用
    void setState(StateT state) { _state = state; }

    void appendReadBuffer(const char *data, size_t len) 
    {
        _readBuffer.append(data, len);
        _readBytes += len;
    }

    void appendWriteBuffer(const char *data, size_t len)
    {
        _writeBuffer.append(data, len);
        _writeBytes += len;
    }

    void enterEventLoop()
    {
        MutexLockGuard lock(_mutex);
        ++_pendingEvent;
    }
    
    size_t leaveEventLoop()
    {
        MutexLockGuard lock(_mutex);
        if (_pendingEvent > 0)
            --_pendingEvent;
        return _pendingEvent;
    }

    size_t pendingEvent() const { return _pendingEvent; }
    size_t readBytes() const { return _readBytes; }
    size_t writeBytes() const { return _writeBytes; }

	void send(const char *data, size_t len)
	{
		appendWriteBuffer(data, len);
		postWriteEvent();
	}
	void send(const std::string &data)
	{ send(data.c_str(), data.length()); }

	void setPostWriteEventFunction(const PostEventFunction &func)
	{ _postWriteEvent = func; }
    void postWriteEvent() { _postWriteEvent(); }

private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(TcpConnection);
    int32_t _connId;
    SOCKET _socket;
    InetAddress _peerAddr;
    Buffer _readBuffer;
    Buffer _writeBuffer;
    size_t _pendingEvent; // 未完成的事件
    StateT _state;
    MutexLock _mutex;
    void *_context;

    // 统计信息
    size_t _readBytes;
    size_t _writeBytes;

	PostEventFunction _postWriteEvent;
};

} // namespace thefox

#endif // _THEFOX_NET_TCPCONNECTION_H_