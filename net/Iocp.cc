#pragma warning(disable:4996) // 禁用warning 4996

#include <net/Iocp.h>
#include <net/Buffer.h>
#include <net/CpEvent.h>
#include <net/EventLoop.h>
#include <net/Connection.h>

namespace thefox
{

void defaultConnectionCallback(const ConnectionPtr &conn)
{
    return;
}

void defaultMessageCallback(const ConnectionPtr &conn, Buffer *buffer, Timestamp recvTime)
{
    buffer->retrieveAll();
}

} // namespace thefox

using namespace thefox;

Iocp::Iocp(EventLoop *eventloop, const std::string &nameArg)
    : _eventloop(eventloop)
    , _name(nameArg)
    , _started(false)
    , _connectionCallback(defaultConnectionCallback)
    , _messageCallback(defaultMessageCallback)
    , _writeCompleteCallback(NULL)
    , _closeCallback(NULL)
{
	_acceptorThread.reset(
		new Thread(std::bind(&Iocp::acceptorLoop, this), "acceptorloop"));
}

Iocp::~Iocp()
{
	_acceptorThread->stop();
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

	_acceptorThread->start();
    // LOG_INFO << tcpserver start done;
    return true;
}

void Iocp::send(const ConnectionPtr &conn, const char *data, size_t len)
{
    if (NULL != conn)
        conn->send(data, len);
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

bool Iocp::isOpen()
{
	if (_started && !_connections.empty()) {
		if (Connection::kConnected == _connections.begin()->second->state())
			return true;
	}
	return false;
}

void Iocp::close()
{    
    _started = false;

    if (!_connections.empty())
        postCloseEvent(_connections.begin()->second);
}

void Iocp::send(const char *data, size_t len)
{
    MutexLockGuard lock(_connMutex);
    if (!_connections.empty())
        send(_connections.begin()->second, data, len);
}

void Iocp::newConnection(SOCKET socket, const InetAddress &peerAddr)
{
	int32_t connId = _nextConnId.inc();
    ConnectionPtr conn(new Connection(socket, connId, peerAddr));
	conn->setPostWriteEventFunction(std::bind(&Iocp::postWriteEvent, this, conn, static_cast<CpEvent *>(NULL)));
    conn->setState(Connection::kConnecting);
    _connections[connId] = conn;
    _eventloop->registerHandle((HANDLE)socket);
    
    if (NULL != conn) {
        conn->setState(Connection::kConnected);
        postZeroByteReadEvent(conn);
        postReadEvent(conn);
        handleConnection(conn);
    }
}

void Iocp::removeConnection(ConnectionPtr conn)
{
    if (INVALID_SOCKET != conn->socket()) {
        CancelIo((HANDLE)conn->socket());
        closesocket(conn->socket());
    }

    if (0 == conn->leaveEventLoop()) {
        conn->setState(Connection::kDisconnecting);
        handleClose(conn);
        conn->setState(Connection::kDisconnected);
        
        MutexLockGuard lock(_connMutex);
        _connections.erase(conn->connId());
	}
}

void Iocp::postReadEvent(const ConnectionPtr &conn, CpEvent *e)
{
    if (NULL == e) {
        conn->enterEventLoop();
        e = CpEventPool::instance()->get(conn);
    }
    e->setEventType(kEventTypeRead);
    e->setEventCallback(std::bind(&Iocp::handleCpRead, this, _1),
		std::bind(&Iocp::handleCpError, this, _1));
    e->resetBuffer();

    DWORD nBytes = 0;
    DWORD flags = 0;
    int bytesRecv = WSARecv(conn->socket(), 
                            &e->wsaBuffer(), 
                            1, 
                            &nBytes, 
                            &flags, 
                            &e->_overlapped, 
                            NULL);
    if (SOCKET_ERROR == bytesRecv && WSA_IO_PENDING != WSAGetLastError()) {
        CpEventPool::instance()->put(e);
        removeConnection(conn);
    }
}

void Iocp::postWriteEvent(const ConnectionPtr &conn, CpEvent *e)
{
    size_t writeable = conn->writeBuffer()->readableBytes();
    if (writeable > 0) {
        int len = (writeable < CpEvent::kMaxBufSize) ? writeable : CpEvent::kMaxBufSize;
        
        if (NULL == e) {
            conn->enterEventLoop();
            e = CpEventPool::instance()->get(conn);
        }
        e->setEventType(kEventTypeWrite);
        e->setEventCallback(std::bind(&Iocp::handleCpWrite, this, _1), 
			std::bind(&Iocp::handleCpError, this, _1));
        e->setBuffer(conn->writeBuffer()->peek(), len);

        DWORD nBytes = 0;
        DWORD flags = 0;
        int byteSend = WSASend(conn->socket(), 
                                &e->wsaBuffer(), 
                                1, 
                                &nBytes, 
                                flags, 
                                &e->_overlapped, 
                                NULL);
        if ((SOCKET_ERROR == byteSend) && (WSA_IO_PENDING != WSAGetLastError())) {
            CpEventPool::instance()->put(e);
            removeConnection(conn);
        }
    } else {
        CpEventPool::instance()->put(e);
        handleWriteComplete(conn);
        conn->leaveEventLoop();
    }
}

void Iocp::postZeroByteReadEvent(const ConnectionPtr &conn, CpEvent *e)
{
    if (NULL == e) {
        conn->enterEventLoop();
        e = CpEventPool::instance()->get(conn);
    }
    e->setEventType(kEventTypeZeroByteRead);
    e->setEventCallback(std::bind(&Iocp::handleCpZeroByteRead, this, _1),
		std::bind(&Iocp::handleCpError, this, _1));
    e->setZeroByteBuffer();

    DWORD nBytes = 0;
    DWORD flags = 0;
    int bytesRecv = WSARecv(conn->socket(), 
                            &e->wsaBuffer(), 
                            1, 
                            &nBytes, 
                            &flags, 
                            &e->_overlapped, 
                            NULL);
    if (SOCKET_ERROR == bytesRecv && WSA_IO_PENDING != WSAGetLastError()) {
        CpEventPool::instance()->put(e);
        removeConnection(conn);
    }
}

void Iocp::postCloseEvent(const ConnectionPtr &conn)
{
    conn->enterEventLoop();
    CpEvent *e = CpEventPool::instance()->get(conn);
    e->setEventType(kEventTypeClose);
    e->setEventCallback(std::bind(&Iocp::handleCpError, this, _1),
		std::bind(&Iocp::handleCpError, this, _1));
    _eventloop->postEvent(e);
}


void Iocp::handleCpError(IoEvent *evt)
{
    CpEvent *se = static_cast<CpEvent *>(evt);
    // LOG_ERROR<<socket error;
	removeConnection(se->conn());
    CpEventPool::instance()->put(se);
}

void Iocp::handleCpRead(IoEvent *evt)
{
    CpEvent *se = static_cast<CpEvent *>(evt);
    ConnectionPtr conn = se->conn();

    if (0 == se->bytesTransfered()) {
		removeConnection(conn);
        CpEventPool::instance()->put(se);
    } else {
        conn->appendReadBuffer(se->wsaBuffer().buf, se->bytesTransfered());
        handleMessage(
            conn, conn->readBuffer(), Timestamp(Timestamp::now()));
        postReadEvent(conn, se);
    }
}

void Iocp::handleCpWrite(IoEvent *evt)
{
    CpEvent *se = static_cast<CpEvent *>(evt);
    
    se->conn()->writeBuffer()->retrieve(se->bytesTransfered());
    postWriteEvent(se->conn(), se);
}

void Iocp::handleCpZeroByteRead(IoEvent *evt)
{
    CpEvent *se = static_cast<CpEvent *>(evt);
    postZeroByteReadEvent(se->conn(), se);
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