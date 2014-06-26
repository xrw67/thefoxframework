/*
 * @brief tcp server for asio
 * @author macwe1024 at gmail dot com
 */

#ifndef _THEFOX_NET_ASIO_TCPSERVER_H_
#define _THEFOX_NET_ASIO_TCPSERVER_H_

#include <map>
#include <base/mutex.h>
#include <base/atomic_integer.h>
#include <net/asio/net_asio.h>

namespace thefox {
namespace net_asio {

class TcpServer
{
public:
    TcpServer(io_service &io, 
			ip::tcp::endpoint listenAddr, 
			const string &nameArg);
    ~TcpServer(void);
	
    /// @brief start the server
    /// @return return true if success，then return false
    bool start();
    
    /// @brief check if the server started
    /// @return return true if success，then return false
	bool started() const { return _started; }

    void setConnectionCallback(const ConnectionCallback &cb)
    { 
        _connectionCallback = cb; 
    }
    void setMessageCallback(const MessageCallback &cb)
    {
        _messageCallback = cb;
    }
    void setWriteCompleteCallback(const WriteCompleteCallback &cb)
    {
        _writeCompleteCallback = cb; 
    }

private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(TcpServer);

	void doAccept();
	void removeConnection(const TcpConnectionPtr &conn);

	typedef std::map<int32_t, TcpConnectionPtr> ConnectionMap;

	const string _name;
	io_service &_io;
	ip::tcp::acceptor _acceptor;

	bool _started;
    AtomicInt32 _nextConnId;
    Mutex _mutex;
	ConnectionMap _connections;

	ConnectionCallback _connectionCallback;
    MessageCallback _messageCallback;
    WriteCompleteCallback _writeCompleteCallback;
};

} // namespace net_asio
} // namespace thefox

#endif // _THEFOX_NET_ASIO_TCPSERVER_H_
