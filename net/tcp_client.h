/*
 * @brief tcp client
 * @author macwe1024 at gmail dot com
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
        
    /// @brief connect to server
    /// @return return true if success，then return false
    bool open(const InetAddress &serverAddr);
        
    /// @brief close the client
    void close();
        
    /// @brief check if connection is opened
    /// @return return true if success，then return false
    bool opened();
    
    /// @brief send data
    void send(const char *data, size_t len);
    void send(const std::string &data);

    void setConnectionCallback(const ConnectionCallback &cb)
    { _connectionCallback = cb; }
    void setMessageCallback(const MessageCallback &cb)
    { _messageCallback = cb; }
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