#include <net/TcpServer.h>
#include <net/Acceptor.h>
#include <net/TcpConnection.h>
#include <net/IoCompletionPort.h>

using namespace thefox;

void defaultConnectionCallback(const TcpConnectionPtr& conn)
{
}

void defaultMessageCallback(const TcpConnectionPtr& conn, Buffer* buffer,Timestamp receiveTime)
{
	buffer->retrieveAll;
}

TcpServer::TcpServer(const InetAddress &listenAddr, const String &nameArg)
	: _hostport(listenAddr.toIpPort())
	, _name(nameArg)
	, _iocp(new IoCompletionPort())
	, _acceptor(new Acceptor(_iocp, listenAddr))
	, _connectionCallback(defaultConnectionCallback)
	, _messageCallback(defaultMessageCallback)
	, _nextConnId(1)
{
	_acceptor->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, _1, _2));
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
		
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	int threads = 2 * si.dwNumberOfProcessors;
	
	for (int i = 0; i < threads; ++i) {
		EventloopThread *loop = new EventloopThread();
		loop->strarLoop(_iocp);
	}
}

void TcpServer::newConnection(SOCKET socket, const InetAddress &localAddr, const InetAddress &peerAddr)
{
	char buf[32];
	snprintf(buf, sizeof(buf), ":%s#%d", _hostport.c_str(), _nextConnId);
	++_nextConnId;
	string connName = _name + buf;
  
	TcpConnectionPtr conn(new TcpConnection(connName, socket, localAddr, peerAddr));
	_connections[connName] = conn;
	conn->setConnectionCallback(connectionCallback_);
	conn->setMessageCallback(messageCallback_);
	conn->setWriteCompleteCallback(writeCompleteCallback_);
	conn->setCloseCallback(boost::bind(&TcpServer::removeConnection, this, _1)); // FIXME: unsafe
	_iocp->assocHandle(socket, conn);
}
