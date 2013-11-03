#include <net/TcpServer.h>
//#include <base/Logging.h>
#include <net/Acceptor.h>
#include <net/EventLoop.h>
#include <net/EventLoopThreadPool.h>
#include <net/SocketsOps.h>
#include <stdio.h>  // snprintf

using namespace thefox;

TcpServer::TcpServer(EventLoop* loop,
                     const InetAddress& listenAddr,
                     const string& nameArg,
                     Option option)

  : _loop(CHECK_NOTNULL(loop))
  , _hostport(listenAddr.toIpPort())
  , _name(nameArg)
  , _acceptor(new Acceptor(loop, listenAddr, option == kReusePort))
  , _threadPool(new EventLoopThreadPool(loop))
  , _connectionCallback(defaultConnectionCallback)
  , _messageCallback(defaultMessageCallback)
  , _nextConnId(1)

{
	_acceptor->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, _1, _2));
}

TcpServer::~TcpServer()
{
	loop_->assertInLoopThread();
	//LOG_TRACE << "TcpServer::~TcpServer [" << name_ << "] destructing";

	for (ConnectionMap::iterator it(_connections.begin()); it != _connections.end(); ++it)
	{
		TcpConnectionPtr conn = it->second;
		it->second.reset();
		conn->getLoop()->runInLoop(
		std::bind(&TcpConnection::connectDestroyed, conn));
		conn.reset();
	}
}

void TcpServer::setThreadNum(int numThreads)
{
	//assert(0 <= numThreads);
	_threadPool->setThreadNum(numThreads);
}

void TcpServer::start()
{
	if (_started.getAndSet(1) == 0)
	{
		_threadPool->start(_threadInitCallback);

		//assert(!acceptor_->listenning());
		_loop->runInLoop(std::bind(&Acceptor::listen, get_pointer(_acceptor)));
	}
}

void TcpServer::newConnection(int sockfd, const InetAddress& peerAddr)
{
	_loop->assertInLoopThread();
	EventLoop* ioLoop = _threadPool->getNextLoop();
	char buf[32];
	_snprintf(buf, sizeof(buf), ":%s#%d", _hostport.c_str(), _nextConnId);
	++_nextConnId;
	String connName = _name + buf;

	LOG_INFO << "TcpServer::newConnection [" << _name
			 << "] - new connection [" << connName
		     << "] from " << peerAddr.toIpPort();
	
	InetAddress localAddr(sockets::getLocalAddr(sockfd));
	// FIXME poll with zero timeout to double confirm the new connection
	// FIXME use make_shared if necessary
	TcpConnectionPtr conn(new TcpConnection(ioLoop, connName, sockfd, localAddr, peerAddr));
	_connections[connName] = conn;
	conn->setConnectionCallback(_connectionCallback);
	conn->setMessageCallback(_messageCallback);
	conn->setWriteCompleteCallback(_writeCompleteCallback_;
	conn->setCloseCallback(std::bind(&TcpServer::removeConnection, this, _1)); // FIXME: unsafe
	ioLoop->runInLoop(boost::bind(&TcpConnection::connectEstablished, conn));
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn)
{
	// FIXME: unsafe
	_loop->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn)
{
	_loop->assertInLoopThread();
	LOG_INFO << "TcpServer::removeConnectionInLoop [" << _name
			 << "] - connection " << conn->name();
	size_t n = _connections.erase(conn->name());
	(void)n;
	assert(n == 1);
	EventLoop* ioLoop = conn->getLoop();
	ioLoop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
}
