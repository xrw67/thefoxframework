#include <boost/bind.hpp>
#include <net/TcpServer.h>
#include <net/Acceptor.h>
#include <net/TcpConnection.h>
#include <net/IoCompletionPort.h>
#include <net/Eventloop.h>

using namespace thefox;

void defaultConnectionCallback(const TcpConnectionPtr& conn)
{
}

void defaultMessageCallback(const TcpConnectionPtr& conn, Buffer* buffer,Timestamp receiveTime)
{
	buffer->retrieveAll();
}

TcpServer::TcpServer(const InetAddress &listenAddr, const String &nameArg)
	: _hostport(listenAddr.toIpPort())
	, _name(nameArg)
	, _acceptor(new Acceptor(listenAddr))
	, _connectionCallback(defaultConnectionCallback)
	, _messageCallback(defaultMessageCallback)
	, _nextConnId(1)
{
	_iocp.registerHandle((HANDLE)_acceptor.get()->getSocketHandle(), 0);
	_acceptor->setNewConnectionCallback(boost::bind(&TcpServer::newConnection, this, _1, _2));
}

TcpServer::~TcpServer()
{
	for (ConnectionMap::iterator it(_connections.begin()); it != _connections.end(); ++it) {
		TcpConnectionPtr conn = it->second;
		it->second.reset();
	}
}

void TcpServer::start()
{
	if (_started)
		return;
	
	_started = true;
	
	// 创建工作线程
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	int threadNumber = 2 * si.dwNumberOfProcessors + 2;
	
	for (int i = 0; i < threadNumber; ++i)
		::CreateThread(NULL, 0, IoWorkThread, this, 0, 0);
}

DWORD WINAPI TcpServer::IoWorkThread(LPVOID serverPtr)
{
	Eventloop loop(reinterpret_cast<TcpServer *>(serverPtr));
	loop.loop();
}

void TcpServer::newConnection(SOCKET socket, const InetAddress &localAddr, const InetAddress &peerAddr)
{
	char buf[32];
	String connName;
	{
		MutexLockGuard lock(_connectionMapLock);
		_snprintf(buf, sizeof(buf), ":%s#%d", _hostport.c_str(), _nextConnId);
		++_nextConnId;
		connName = _name + buf;
	}
	
	TcpConnectionPtr conn(new TcpConnection(this, connName, socket, localAddr, peerAddr));
	_connections[connName] = conn;
	conn->setConnectionCallback(_connectionCallback_);
	conn->setMessageCallback(_messageCallback_);
	conn->setWriteCompleteCallback(_writeCompleteCallback_);
	conn->setCloseCallback(boost::bind(&TcpServer::removeConnection, this, _1));
	_iocp.registerHandle(socket, conn);
}

void TcpServer::removeConnection(const TcpConnection &conn)
{

}