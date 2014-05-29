/*
* @filename TcpConnection.h
* @brief 表示一个客户连接
* @author macwe@qq.com
*/

#ifndef _THEFOX_NET_TCPCONNECTION_H_
#define _THEFOX_NET_TCPCONNECTION_H_

#include <base/common.h>
#include <base/mutex.h>
#include <net/buffer.h>
#include <net/inet_address.h>
#include <net/callbacks.h>
#include <net/socket.h>
#include <net/event.h>

namespace thefox
{

#ifndef WIN32
typedef int SOCKET
#endif

class EventLoop;

class TcpConnection
{
public:
    enum StateT { kDisconnected, kConnecting, kConnected, kDisconnecting };

    TcpConnection(EventLoop *loop, SOCKET sockfd, int id, 
		const InetAddress& localAddr, const InetAddress &peerAddr);
    ~TcpConnection();

    /// @brief 获取连接ID
    int32_t id() const { return _id; }

    /// @brief 获取SOCKET句柄
    SOCKET fd() const { return _socket.fd(); }

    /// @brief 获取对端ip地址
    const InetAddress &peerAddr() const { return _peerAddr; }

    /// @brief 获取Read缓冲区指针
    Buffer *readBuffer() { return &_readBuffer; }

    /// @brief 获取Write缓冲区指针
    Buffer *writeBuffer() { return &_writeBuffer; }

    /// @brief 获取连接状态
    StateT state() const { return _state; }
	void setState(StateT state) { _state = state; }

    /// @brief 
    void *arg() const { return _arg; }

    /// @brief 
	void setArg(void *arg) { _arg = arg; }

    size_t readBytes() const { return _readBytes; }
    size_t writeBytes() const { return _writeBytes; }

	void send(const std::string &data);
	void send(const char *data, size_t len);

	bool shutdown();
	void setTcpNoDelay(bool on);
	void forceClose();
	void connectEstablished();
	void connectDestroyed();

    void setConnectionCallback(const ConnectionCallback &cb)
	{ _connectionCallback = cb; }
    void setMessageCallback(const MessageCallback &cb)
    { _messageCallback = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback &cb)
    { _writeCompleteCallback = cb; }
	void setRemoveConnectionCallback(const RemoveConnectionCallback &cb)
	{ _removeConnectionCallback = cb; }

	void addReadBytes(size_t bytes)
	{ _readBytes += bytes;}
	void addWriteBytes(size_t bytes)
	{ _writeBytes += bytes; }

private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(TcpConnection);
	EventLoop *_loop;
    int32_t _id;
    Socket _socket;
	const InetAddress _localAddr;
    const InetAddress _peerAddr;
    Buffer _readBuffer;
    Buffer _writeBuffer;
	Mutex _mutex;
    StateT _state;

	IoEvent _event;
    void *_arg;

    // 统计信息
    size_t _readBytes;
    size_t _writeBytes;

	ConnectionCallback _connectionCallback;
    MessageCallback _messageCallback;
    WriteCompleteCallback _writeCompleteCallback;
	RemoveConnectionCallback _removeConnectionCallback;

#ifdef WIN32
	friend class IocpEvent;
#else
	friend class EpollEvent;
#endif
};

} // namespace thefox

#endif // _THEFOX_NET_TCPCONNECTION_H_