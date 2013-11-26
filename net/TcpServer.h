#ifndef _THEFOX_NET_SERVER_TCPSERVER_H_
#define _THEFOX_NET_SERVER_TCPSERVER_H_

#include <base/Types.h>
#include <base/noncopyable.h>
#include <base/scoped_ptr.h>
#include <net/winapi.h>
#include <net/Callbacks.h>
#include <net/InetAddress.h>
#include <net/IoCompletionPort.h>

namespace thefox
{

class Buffer;
class Acceptor;
class TcpConnection;

class TcpServer : noncopyable
{
public:
	TcpServer(const InetAddress &listenAddr, const String &nameArg);
	~TcpServer();
	
	/// @brief 启动服务
	void start();
	/// @brief 停止服务
	void stop();
	
	// 设置回调函数
    void setConnectionCallback(const ConnectionCallback &cb) { _connectionCallback = cb; }
    void setMessageCallback(const MessageCallback &cb) { _messageCallback = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback &cb) { _writeCompleteCallback = cb; }
	
private:
	static DWORD WINAPI IoWorkThread(LPVOID serverPtr);

	void newConnection(SOCKET socket, const InetAddress &localAddr, const InetAddress &peerAddr);
	void removeConnection(const TcpConnection &conn);
	
	typedef std::map<String, TcpConnectionPtr> ConnectionMap;
	
	IoCompletionPort _iocp; // 完成端口的封装
	scoped_ptr<Acceptor> _acceptor; // 接收客户端连接
	ConnectionMap _connections; //客户端连接列表
	int _nextConnId;
	
	bool _started;// 服务器是否启动
	const String _hostport;// 服务器的地址
	const String _name;// 服务器的名称
	ConnectionCallback _connectionCallback;
	MessageCallback _messageCallback;
	WriteCompleteCallback _writeCompleteCallback;
	
};

}

#endif // _THEFOX_NET_SERVER_TCPSERVER_H_