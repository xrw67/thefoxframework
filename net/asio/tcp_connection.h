/*
* @filename TcpConnection.h
* @brief 表示一个客户连接
* @author macwe1024 at gmail dot com
*/

#ifndef _THEFOX_NET_ASIO_TCPCONNECTION_H_
#define _THEFOX_NET_ASIO_TCPCONNECTION_H_

#include <net/asio/net_asio.h>

namespace thefox {
namespace net_asio {

typedef std::function<void(const TcpConnectionPtr &conn)> RemoveConnectionCallback;

class TcpConnection : public std::enable_shared_from_this<TcpConnection>
{
public:
    enum StateT { kDisconnected, kConnecting, kConnected, kDisconnecting };

	TcpConnection(io_service &io, int id);
    ~TcpConnection();
	
    /// @brief get conn id
    int32_t id() const { return _id; }

	ip::tcp::socket &socket() { return _socket; }

    /// @brief get peer address
	ip::tcp::endpoint peerAddr() const 
	{
		return _socket.remote_endpoint();
	}

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

	void shutdown();
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
	void doRead();
	void doWrite();

    void connectDestroyed();

    int32_t _id;
	io_service &_io;
	ip::tcp::socket _socket;

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
};

} // namespace net_asio
} // namespace thefox

#endif // _THEFOX_NET_ASIO_TCPCONNECTION_H_