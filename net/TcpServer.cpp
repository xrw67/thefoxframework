#include <net/TcpServer.h>

using namespace thefox;

TcpServer::TcpServer(const int port,
					const string &nameArg,
					Option option)
	: _hostport(port)
	, _name(nameArg)
	, _connectionCallback(defaultConnectionCallback)
	, _messageCallback(defaultMessageCallback)
	, _nextConnId(1)
{
	
	
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
	if (!_started)
	{
		_started = TRUE;
		
		ioCompletionPort = CreateIoCompletePort(INVALID_HANDLE_VALUE, NULL, 0, 0);
		if (NULL == ioCompletionPort)
			return;
		
		SYSTEM_INFO si;
		GetSystemInfo(&si);
		int threads = 2 * si.dwNumberOfProcessors;
		
		
	}
}
