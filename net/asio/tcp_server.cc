#include <net/asio/tcp_server.h>
#include <log/logging.h>

using namespace thefox;
using namespace thefox::net_asio;

TcpServer::TcpServer(io_service &io, 
					boost::asio::ip::tcp::endpoint listenAddr, 
					const string &nameArg)
	: _io(io)
	,_name(nameArg)
	, _started(false)
    , _messageCallback(defaultMessageCallback)
	, _acceptor(io, listenAddr)
{
}

TcpServer::~TcpServer()
{
	THEFOX_TRACE_FUNCTION;

	_started = false;
}

bool TcpServer::start()
{
	THEFOX_TRACE_FUNCTION;

	if (_started)
		return true;
	
	_started = true;

	doAccept();

	return _started;
}

void TcpServer::doAccept()
{
	int32_t id = _nextConnId.inc();
	TcpConnectionPtr newConnection(new TcpConnection(_io, id));

	_acceptor.async_accept(newConnection->socket(), 
		[this, newConnection](const boost::system::error_code &ec) {
			if (!ec) {
				_connections[newConnection->id()] = newConnection;

				newConnection->setConnectionCallback(_connectionCallback);
				newConnection->setMessageCallback(_messageCallback);
				newConnection->setWriteCompleteCallback(_writeCompleteCallback);
				newConnection->setRemoveConnectionCallback(std::bind(&TcpServer::removeConnection, this, _1));
				newConnection->connectEstablished();

				doAccept();
			}
		});
}

void TcpServer::removeConnection(const TcpConnectionPtr &conn)
{
	_connections.erase(conn->id());
}