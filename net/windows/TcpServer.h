#ifndef _THEFOX_NET_SERVER_TCPSERVER_H_
#define _THEFOX_NET_SERVER_TCPSERVER_H_

#include <base/Types.h>
#include <base/noncopyable.h>
#include <net/winapi.h>
#include <net/InetAddress.h>
#include <net/IoCompletionPort.h>
#include <net/IoBufferPool.h>
#include <net/Acceptor.h>

namespace thefox
{

class Buffer;
class Acceptor;
class TcpConnection;

class TcpServer : noncopyable
{
public:
	typedef TcpConnection *TcpConnectionPtr;

	TcpServer(const InetAddress &listenAddr, const String &nameArg);
	~TcpServer();
	
	/// @brief 启动服务
	void start();
	/// @brief 停止服务
	void stop();

	void send(const TcpConnectionPtr &conn, const void *message, size_t len);
	void shutdown(const TcpConnectionPtr &conn);

	virtual void OnNewConnection(SOCKET socket, const InetAddress &localAddr, const InetAddress &peerAddr);
	virtual void OnConnectionClose(const TcpConnectionPtr &conn);
	virtual void OnConnectionError(const TcpConnectionPtr &conn);
	virtual void OnReadCompleted(const TcpConnectionPtr &conn, Buffer *buf, Timestamp ts);
	virtual void OnWriteCompleted(const TcpConnectionPtr &conn);
	
	void TcpServer::IoProcess(const TcpConnectionPtr &conn, IoBuffer *buf);

	static DWORD WINAPI AcceptThread(LPVOID serverPtr); // 接收者线程
	static DWORD WINAPI IoWorkThread(LPVOID serverPtr); // IO工作线程
private:
	void removeConnection(const TcpConnectionPtr &conn);
	
	typedef std::map<String, TcpConnectionPtr> ConnectionMap;
	
	IoCompletionPort _iocp; // 完成端口的封装
	Acceptor _acceptor; // 接收客户端连接
	IoBufferPool _ioBufferPool;
	MutexLock _connectionMapLock;
	ConnectionMap _connections; //客户端连接列表
	uint32_t _nextConnId;
	
	bool _started;// 服务器是否启动
	const String _hostport;// 服务器的地址
	const String _name;// 服务器的名称
};

}

#endif // _THEFOX_NET_SERVER_TCPSERVER_H_