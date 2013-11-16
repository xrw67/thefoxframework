#include <net/TcpServer.h>

using namespace thefox;

TcpServer::TcpServer(const int port,
					const string &nameArg,
					Option option)
	: _hostport(port)
	, _name(nameArg)
	, _acceptor(new Acceptor())
	, _connectionCallback(defaultConnectionCallback)
	, _messageCallback(defaultMessageCallback)
	, _nextConnId(1)
{
	_acceptor->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, _1, _2));
}

TcpServer::~TcpServer()
{
	for (ConnectionMap::iterator it(_connections.begin()));
	it != _connections.end(); ++it)
	{
		TcpConnectionPtr conn = it->second;
		it->second.reset();
		
	}
}

void TcpServer::start()
{
	if (_started)
	{
		return;
	}
	
		_started = TRUE;
		
		
		if ((_completionPort = CreateIoCompletePort(INVALID_HANDLE_VALUE, NULL, 0, 0)) == NULL)
			return;
		
		SYSTEM_INFO si;
		GetSystemInfo(&si);
		int threads = 2 * si.dwNumberOfProcessors;
		
		for (int i = 0; i < threads; ++i)
		{
			EventloopThread *loop = new EventloopThread(this);
			loop->strarLoop();
		}

		if (NULL == CreateIoCompletionPort(_acceptor->_socket, _completionPort, (DWORD)_acceptor, 0))  
		{  
			RELEASE_SOCKET( m_pListenContext->m_Socket );
			return;
		}

		

		
	}
}

void TcpServer::newConnection(SOCKET socket, const InetAddress& peerAddr)
{
	char buf[32];
	snprintf(buf, sizeof(buf), ":%s#%d", _hostport.c_str(), _nextConnId);
	++_nextConnId;
	string connName = _name + buf;

	InetAddress localAddr(sockets::getLocalAddr(socket));
  
	TcpConnectionPtr conn(new TcpConnection(connName, socket, localAddr, peerAddr));
	_connections[connName] = conn;
	conn->setConnectionCallback(connectionCallback_);
	conn->setMessageCallback(messageCallback_);
	conn->setWriteCompleteCallback(writeCompleteCallback_);
	conn->setCloseCallback(boost::bind(&TcpServer::removeConnection, this, _1)); // FIXME: unsafe
	ioLoop->runInLoop(boost::bind(&TcpConnection::connectEstablished, conn));
  
	if (NULL == CreateIoCompletionPort((HANDLE)socket, _completionPort, (DWORD)conn, 0))
	{
		// failed
	}
}
