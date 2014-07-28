/*
* @filename tcp_connection.h
* @brief 表示一个客户连接
* @author macwe1024 at gmail dot com
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
#include <net/event_loop.h>

namespace thefox {
namespace net {

class EventLoop;
class TcpConnection;
typedef std::function<void(TcpConnection *conn)> RemoveConnectionCallback;

class TcpConnection
{
public:
    enum StateT { kDisconnected, kConnecting, kConnected, kDisconnecting };

    TcpConnection(EventLoop *loop, SOCKET sockfd, int id, 
                  const InetAddress &localAddr, const InetAddress &peerAddr);
    ~TcpConnection();
	
    /// @brief get conn id
    int32_t id() const { return _id; }

    /// @brief get socket handle
    SOCKET fd() const { return _socket.fd(); }

    /// @brief get peer address
    const InetAddress &peerAddr() const { return _peerAddr; }

    Buffer *readBuffer() { return &_readBuffer; }
    Buffer *writeBuffer() { return &_writeBuffer; }

    /// @brief get connection state
    StateT state() const { return _state; }
	void setState(StateT state) { _state = state; }

    /// @brief 
    void *arg() const { return _arg; }

    /// @brief 
	void setArg(void *arg) { _arg = arg; }

    size_t readBytes() const { return _readBytes; }
    size_t writeBytes() const { return _writeBytes; }

	void send(const string &data);
	void send(const char *data, size_t len);

	bool shutdown();
	void setTcpNoDelay(bool on);
	void forceClose();
	void connectEstablished();

    void setConnectionCallback(const ConnectionCallback &cb)
	{ _connectionCallback = cb; }
    void setMessageCallback(const MessageCallback &cb)
    { _messageCallback = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback &cb)
    { _writeCompleteCallback = cb; }
	void setRemoveConnectionCallback(const RemoveConnectionCallback &cb)
	{ _removeConnectionCallback = cb; }

private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(TcpConnection);

    void connectDestroyed();

    void enterEvent();
    size_t leaveEvent();

    void addReadBytes(size_t bytes) { _readBytes += bytes; }
    void addWriteBytes(size_t bytes) { _writeBytes += bytes; }

	EventLoop *_loop;
    int32_t _id;
    Socket _socket;
	const InetAddress _localAddr;
    const InetAddress _peerAddr;
    Buffer _readBuffer;
    Buffer _writeBuffer;
    StateT _state;

    void *_arg;

    // io transfer counter
    size_t _readBytes;
    size_t _writeBytes;

	ConnectionCallback _connectionCallback;
    MessageCallback _messageCallback;
    WriteCompleteCallback _writeCompleteCallback;
	RemoveConnectionCallback _removeConnectionCallback;

    Event _event;
    Mutex _eventmutex;
    size_t _pendingEvents;

	friend class Poller;
};

} // namespace net
} // namespace thefox

#endif // _THEFOX_NET_TCPCONNECTION_H_