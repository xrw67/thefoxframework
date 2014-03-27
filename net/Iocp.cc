#pragma warning(disable:4996)

#include <net/Iocp.h>
#include <net/Buffer.h>
#include <net/SocketEvent.h>
#include <net/EventLoop.h>
#include <net/TcpConnection.h>


namespace thefox
{

void handleError(IoEvent *e)
{
	SocketEvent *se = static_cast<SocketEvent *>(e);
	// LOG_ERROR<<socket error;
	if (NULL != se->conn())
		se->iocp()->removeConnection(se->conn());
	delete e;
}

void handleClose(IoEvent *e)
{
	SocketEvent *se = static_cast<SocketEvent *>(e);
	// LOG_INFO<<socket close;
	se->iocp()->removeConnection(se->conn());
	delete e;
}

void handleRead(IoEvent *e)
{
	SocketEvent *se = static_cast<SocketEvent *>(e);
	if (0 == se->bytesTransfered()) {
		handleClose(e);
	} else {
		se->conn()->appendReadBuffer(se->wsaBuffer().buf, se->bytesTransfered());
		se->iocp()->handleMessage(
			se->conn(), se->conn()->readBuffer(), Timestamp(Timestamp::now()));
		if (!se->iocp()->postReadEvent(se->conn(), se))
			handleError(e);
	}
}

void handleWrite(IoEvent *e)
{
	SocketEvent *se = static_cast<SocketEvent *>(e);
	if (!se->iocp()->postWriteEvent(se->conn(), se))
		handleError(e);
}

void handleZeroByteRead(IoEvent *e)
{
	SocketEvent *se = static_cast<SocketEvent *>(e);
	if (!se->iocp()->postZeroByteReadEvent(se->conn(), se))
		handleError(e);
}

void defaultConnectionCallback(const TcpConnectionPtr &conn)
{

}

void defaultMessageCallback(const TcpConnectionPtr &conn, Buffer *buffer, Timestamp recvTime)
{
	buffer->retrieveAll();
}

// 接收新连接的线程
DWORD WINAPI acceptorThreadProc(LPVOID param)
{
	Iocp *server = reinterpret_cast<Iocp *>(param);
	server->acceptorLoop();
	return 0;
}

} // namespace thefox

using namespace thefox;

Iocp::Iocp(EventLoop *eventloop, const String &nameArg)
    : _eventloop(eventloop)
	, _name(nameArg)
	, _nextConnId(1)
	, _started(false)
	, _connectionCallback(defaultConnectionCallback)
	, _messageCallback(defaultMessageCallback)
	, _writeCompleteCallback(NULL)
	, _closeCallback(NULL)
{
}

Iocp::~Iocp()
{
	close();
}

bool Iocp::start(const InetAddress &listenAddr)
{
	if (started()) {
		// LOG_WARN << tcpserver already started;
		return false;
	}
	_started = true;

    _socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET  == _socket) {
		int errCode = WSAGetLastError();
		// LOG_ERROR << WSASocket failed!, errcode:<< errCode;
		_started = false;
		return false;
	}
    _hAcceptEvent = WSACreateEvent();
    WSAEventSelect(_socket, _hAcceptEvent, FD_ACCEPT);
    int ret = bind(_socket, (struct sockaddr *)&listenAddr.getSockAddrInet(), sizeof(struct sockaddr_in));
	if (SOCKET_ERROR   == ret) {
		int errCode = WSAGetLastError();
		// LOG_ERROR << WSASocket failed!, errcode:<< errCode;
		closesocket(_socket);
		_started = false;
		return false;
	}
    ret = listen(_socket, 200);
	if (SOCKET_ERROR   == ret) {
		int errCode = WSAGetLastError();
		// LOG_ERROR << WSASocket failed!, errcode:<< errCode;
		closesocket(_socket);
		_started = false;
		return false;
	}
    HANDLE handle = CreateThread(NULL, 0, acceptorThreadProc, this, 0, NULL);
    CloseHandle(handle);

	// LOG_INFO << tcpserver start done;
	return true;
}

void Iocp::send(const TcpConnectionPtr &conn, const char *data, size_t len)
{
	if (NULL != conn) {
		conn->appendWriteBuffer(data, len);
		postWriteEvent(conn);
	}
}

bool Iocp::open(const InetAddress &serverAddr)
{
	if (started()) {
		// LOG_WARN << tcpserver already started;
		return false;
	}
	_started = true;

	_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET  == _socket) {
		int errCode = WSAGetLastError();
		// LOG_ERROR << WSASocket failed!, errcode:<< errCode;
		_started = false;
		return false;
	}

	int ret = connect(_socket, 
					  (struct sockaddr *)&serverAddr.getSockAddrInet(), 
					  sizeof(struct sockaddr_in));
	if (SOCKET_ERROR == ret && WSAEWOULDBLOCK != WSAGetLastError()) {
		// LOG_ERROR 
		closesocket(_socket);
		_started = false;
		return false;
	}
	newConnection(_socket, serverAddr);
	return true;
}

void Iocp::close()
{	
	_started = false;

	while (!_connections.empty())
		removeConnection(_connections.begin()->second);
}

void Iocp::send(const char *data, size_t len)
{
	MutexLockGuard lock(_connMutex);
	if (!_connections.empty())
		send(_connections.begin()->second, data, len);
}

void Iocp::newConnection(SOCKET socket, const InetAddress &peerAddr)
{
	TcpConnectionPtr conn = NULL;
	{
		MutexLockGuard lock(_connMutex);
		int32_t connId = _nextConnId;
		++_nextConnId;

		conn = new TcpConnection(socket, connId, peerAddr);
		conn->setState(TcpConnection::kConnecting);
		_connections[connId] = conn;
		_eventloop->registerHandle((HANDLE)socket);
	}
	
	if (NULL != conn) {
		conn->setState(TcpConnection::kConnected);
		handleConnection(conn);

		if (!postReadEvent(conn) || !postZeroByteReadEvent(conn))
			removeConnection(conn);
	}
}

void Iocp::removeConnection(TcpConnectionPtr conn)
{
	MutexLockGuard lock(_connMutex);

	if (NULL == conn)
		return;

	conn->setState(TcpConnection::kDisconnecting);
	this->handleClose(conn);
	conn->setState(TcpConnection::kDisconnected);
	
	if(_connections.erase(conn->connId() > 0)) {
		delete conn;
		conn = NULL;
	}
}

bool Iocp::postReadEvent(const TcpConnectionPtr &conn, SocketEvent *e)
{
	if (NULL == e)
		e = new SocketEvent(this, conn);
	
	e->setEventType(kEventTypeCpRead);
	e->setEventCallback(handleRead, handleError);
	e->resetBuffer();

	DWORD nBytes = 0;
	DWORD flags = 0;
	int bytesRecv = WSARecv(
		conn->socket(), &e->wsaBuffer(), 1, &nBytes, &flags, &e->_overlapped, NULL);
	if (SOCKET_ERROR == bytesRecv && WSA_IO_PENDING != WSAGetLastError()) {
		delete e;
		return false;
	}
	return true;
}

bool Iocp::postWriteEvent(const TcpConnectionPtr &conn, SocketEvent *e)
{
	Buffer *buf = conn->writeBuffer();
	int writeable = buf->readableBytes();
	if (writeable > 0) {
		int len = (writeable < SocketEvent::kMaxBufSize) ? writeable : SocketEvent::kMaxBufSize;
		if (NULL == e)
			e = new SocketEvent(this, conn);
	
		e->setEventType(kEventTypeCpWrite);
		e->setEventCallback(handleWrite, handleError);
		e->setBuffer(buf->peek(), len);

		DWORD nBytes = 0;
		DWORD flags = 0;
		int byteSend = WSASend(
			conn->socket(), &e->wsaBuffer(), 1, &nBytes, flags, &e->_overlapped, NULL);
		if ((SOCKET_ERROR == byteSend) && (WSA_IO_PENDING != WSAGetLastError())) {
			delete e;
			return false;
		} else {
			buf->retrieve(len);
		}

		if (0 == buf->readableBytes())
			handleWriteComplete(conn);
	}
	return true;
}

bool Iocp::postZeroByteReadEvent(const TcpConnectionPtr &conn, SocketEvent *e)
{
	if (NULL == e)
		e = new SocketEvent(this, conn);
	
	e->setEventType(kEventTypeCpZeroByteRead);
	e->setEventCallback(handleZeroByteRead, handleError);
	e->setZeroByteBuffer();

	DWORD nBytes = 0;
	DWORD flags = 0;
	int bytesRecv = WSARecv(
		conn->socket(), &e->wsaBuffer(), 1, &nBytes, &flags, &e->_overlapped, NULL);
	if (SOCKET_ERROR == bytesRecv && WSA_IO_PENDING != WSAGetLastError()) {
		delete e;
		return false;
	}
	return true;
}

void Iocp::acceptorLoop()
{
    while (started()) {
        if (WSA_WAIT_TIMEOUT != WSAWaitForMultipleEvents(1,&_hAcceptEvent, FALSE, 100, FALSE)) {
			WSANETWORKEVENTS events;
			if (SOCKET_ERROR != WSAEnumNetworkEvents(_socket, _hAcceptEvent, &events)) {
				if (events.lNetworkEvents & FD_ACCEPT && 0 == events.iErrorCode[FD_ACCEPT_BIT]) {
					SOCKET clientSocket = INVALID_SOCKET;
					struct sockaddr_in addr;
					int len = sizeof(addr);
					clientSocket = WSAAccept(_socket, (sockaddr *)&addr, &len, 0, 0);
					if (SOCKET_ERROR != clientSocket)
						newConnection(clientSocket, InetAddress(addr));
				}
			}
        }
    }
}