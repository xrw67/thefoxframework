#ifndef _THEFOX_NET_IOCP_H_
#define _THEFOX_NET_IOCP_H_

#include <map>
#include <base/noncopyable.h>
#include <base/Types.h>
#include <Winsock2.h>
#include <base/MutexLock.h>
#include <net/InetAddress.h>
#include <net/Callbacks.h>

namespace thefox
{

class IoContext;
class TcpConnection;
typedef IoContext *IoContextPtr;
typedef std::map<int32_t, TcpConnectionPtr> ConnectionMap;

class Iocp : noncopyable
{
public:
	Iocp(const String &nameArg);
	~Iocp();
    bool start(const InetAddress &listenAddr);
	bool started() { return _started; }
	void send(const TcpConnectionPtr &conn, const char *data, size_t len);
	void removeConnection(const TcpConnectionPtr &conn);
    void setConnectionCallback(const ConnectionCallback &cb);
    void setCloseCallback(const CloseCallback &cb);
    void setMessageCallback(const MessageCallback &cb);
	void setWriteCompleteCallback(const WriteCompleteCallback &cb);

	// 工作者循环
	void workerLoop();
    // 接受者循环
    void acceptorLoop();

	// 客户端使用的方法
	bool open(const InetAddress &serverAddr);
	void close();
	bool isOpen() { return _started; }
	void send(const char *data, size_t len);
private:
	void newConnection(SOCKET socket, const InetAddress &peerAddr);
	void handleRead(const TcpConnectionPtr &conn, IoContextPtr io);
	void handleWrite(const TcpConnectionPtr &conn, IoContextPtr io = NULL);
    bool initIocp();
	int getCpuNum();

	const String _name;
    SOCKET _socket;
	HANDLE _hIocp;
    HANDLE _hAcceptEvent;
    bool _started;
	int32_t _nextConnId;

    ConnectionMap _connections;
	MutexLock _connLock;
    
    ConnectionCallback _connectionCallback;
	CloseCallback _closeCallback;
	MessageCallback _messageCallback;
	WriteCompleteCallback _writeCompleteCallback;
    
    
};

} // namespace thefox

#endif // _THEFOX_NET_IOCP_H_