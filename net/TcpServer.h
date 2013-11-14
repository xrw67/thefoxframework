#ifndef _THEFOX_NET_TCPSERVER_H_
#define _THEFOX_NET_TCPSERVER_H_

#include <base/noncopyable.h>

class Acceptor;

namespace thefox
{

class TcpServer : noncopyable
{
public:
	TcpServer();
	~TcpServer();
	
	void strat();
	
	void stop();
	
    void setConnectionCallback(const ConnectionCallback& cb)
    { connectionCallback_ = cb; }

    void setMessageCallback(const MessageCallback& cb)
    { messageCallback_ = cb; }

    void setWriteCompleteCallback(const WriteCompleteCallback& cb)
    { writeCompleteCallback_ = cb; }
private:
	void newConnection(SOCKET sock, const InetAddress &peerAddr);
	void removeConnection(const TcpConnection &conn);
	
	typedef std::map<string, TcpConnectionPtr> ConnectionMap;
	
	const string _hostport;
	const string _name;
	HANDLE _completionPort;
	std::unique_ptr<Acceptor> _acceptor;
	ConnectionCallback _connectionCallback;
	MessageCallback _messageCallback;
	WriteCompleteCallback _writeCompleteCallback;
	bool _started;
	int _nextConnId;
	ConnectionMap _connections;
};

}

#endif // _THEFOX_NET_TCPSERVER_H_