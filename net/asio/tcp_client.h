/*
 * @brief tcp client
 * @author macwe1024 at gmail dot com
 */

#ifndef _THEFOX_NET_ASIO_TCPCLIENT_H_
#define _THEFOX_NET_ASIO_TCPCLIENT_H_

#include <net/asio/net_asio.h>

namespace thefox {
namespace net_asio {

class TcpClient
{
public:
    TcpClient(io_service &io, const string &nameArg);
    ~TcpClient(void);
        
    /// @brief connect to server
    /// @return return true if success，then return false
    bool open(ip::tcp::endpoint endpoint);
        
    /// @brief close the client
    void close();
        
    /// @brief check if connection is opened
    /// @return return true if success，then return false
    bool opened();
    
    /// @brief send data
    void send(const char *data, size_t len);
    void send(const string &data);

    void setConnectionCallback(const ConnectionCallback &cb)
    { _connectionCallback = cb; }
    void setMessageCallback(const MessageCallback &cb)
    { _messageCallback = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback &cb)
    { _writeCompleteCallback = cb; }

private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(TcpClient);

	const string _name;
	io_service &_io;
	TcpConnectionPtr _conn;
	

	ConnectionCallback _connectionCallback;
    MessageCallback _messageCallback;
    WriteCompleteCallback _writeCompleteCallback;
};
    
} // namespace net_asio
} // namespace thefox

#endif // _THEFOX_NET_ASIO_TCPCLIENT_H_