#ifndef _THEFOX_NET_WIN_IOCP_H_
#define _THEFOX_NET_WIN_IOCP_H_

#ifndef WIN32
#else

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#include "mswsock.h"

#include <map>
#include <list>
#include <base/Types.h>
#include <base/noncopyable.h>
#include <base/net/Callbacks.h>
#include <base/net/win/IocpBuffer.h>
#include <base/net/win/TcpConnection.h>

namespace thefox
{

namespace net
{

class Iocp : noncopyable
{
public:
	Iocp(const InetAddress &listenAddr, const String &name);
	~Iocp();

	void start();
	void shutdown();

	void send(const TcpConnectionPtr &conn, char *data, size_t len);

	const String &hostport() const { return _hostport; }
	const String &name() const { return _name; }

	void setConnectionCallback(const ConnectionCallback &cb)
	{ _connectionCallback = cb; }

	void setMessageCallback(const MessageCallback &cb)
	{ _messageCallback = cb; }

	void setWriteCompleteCallback(const WriteCompleteCallback &cb)
	{ _writeCompleteCallback = cb; }

private:
	static DWORD listenThreadProc(LPVOID param);
	static DWORD ioWorkerThreadProc(LPVOID param);

	void newConnection(SOCKET sock, const InetAddress &peerAddr);
	void removeConnection(const TcpConnectionPtr &conn);

	void ProcessIoMessage(const TcpConnectionPtr &conn, IocpBufferPtr buf, DWORD ioSize);
	void onIoWorkerWriteCompleted(const TcpConnectionPtr &conn, )

	typedef std::map<String, TcpConnectionPtr> ConnectionMap;

	// IO工作现场操作
	inline void onWrited

	const String _hostport;
	const String _name;

	ConnectionCallback _connectionCallback;
	MessageCallback _messageCallback;
	WriteCompleteCallback _writeCompleteCallback;

	bool _started;
	uint32_t _nextConnId;
	ConnectionMap _connections;

	SOCKET _listenSocket;
	HANDLE _completionPort;
	HANDLE _listenThread;
	std::list<HANDLE> _ioWorkerThreads

	std::list<TcpConnectionPtr> _freeConnections;
	MutexLock _freeConnectionsLock;

	std::list<IocpBufferPtr> _freeBuffers;
	MutexLock _freeBuffersLock;
};

typedef Iocp TcpServerImpl, TcpClientImpl;

} // namespace net

} // namespace thefox

#endif // WIN32

#endif // _THEFOX_NET_WIN_IOCP_H_