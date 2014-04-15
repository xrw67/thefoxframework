#pragma warning(disable:4996) // 禁用warning 4996

#include <net/Iocp.h>
#include <net/Buffer.h>
#include <net/CpEvent.h>
#include <net/EventLoop.h>
#include <net/TcpConnection.h>


namespace thefox
{

void handleError(IoEvent *e)
{
    CpEvent *se = static_cast<CpEvent *>(e);
    // LOG_ERROR<<socket error;
    Iocp *iocp = se->iocp();
    TcpConnectionPtr conn = se->conn();

    CpEventPool::instance()->put(se);
    iocp->removeConnection(conn);
}

void handleRead(IoEvent *e)
{
    CpEvent *se = static_cast<CpEvent *>(e);
    Iocp *iocp = se->iocp();
    TcpConnectionPtr conn = se->conn();

    if (0 == se->bytesTransfered()) {
        CpEventPool::instance()->put(se);
        iocp->removeConnection(conn);
    } else {
        conn->appendReadBuffer(se->wsaBuffer().buf, se->bytesTransfered());
        se->iocp()->handleMessage(
            conn, conn->readBuffer(), Timestamp(Timestamp::now()));
        se->iocp()->postReadEvent(conn, se);
    }
}

void handleWrite(IoEvent *e)
{
    CpEvent *se = static_cast<CpEvent *>(e);
    
    se->conn()->writeBuffer()->retrieve(se->bytesTransfered());
    se->iocp()->postWriteEvent(se->conn(), se);
}

void handleZeroByteRead(IoEvent *e)
{
    CpEvent *se = static_cast<CpEvent *>(e);
    se->iocp()->postZeroByteReadEvent(se->conn(), se);
}

void defaultConnectionCallback(const TcpConnectionPtr &conn)
{
    return;
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

bool Iocp::isOpen()
{
	if (_started && !_connections.empty()) {
		if (TcpConnection::kConnected == _connections.begin()->second->state())
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
    TcpConnectionPtr conn = NULL;
    {
        MutexLockGuard lock(_connMutex);
        int32_t connId = _nextConnId;
        ++_nextConnId;

        conn = TcpConnectionPool::instance()->get(socket, connId, peerAddr);
        conn->setState(TcpConnection::kConnecting);
        _connections[connId] = conn;
        _eventloop->registerHandle((HANDLE)socket);
    }
    
    if (NULL != conn) {
        conn->setState(TcpConnection::kConnected);
        postZeroByteReadEvent(conn);
        postReadEvent(conn);
        handleConnection(conn);
    }
}

void Iocp::removeConnection(TcpConnectionPtr conn)
{
    if (INVALID_SOCKET != conn->socket()) {
        CancelIo((HANDLE)conn->socket());
        closesocket(conn->socket());
    }

    if (0 == conn->leaveEventLoop()) {
        conn->setState(TcpConnection::kDisconnecting);
        handleClose(conn);
        conn->setState(TcpConnection::kDisconnected);
        
        MutexLockGuard lock(_connMutex);
        if(0 != _connections.erase(conn->connId()))
            TcpConnectionPool::instance()->put(conn);
    }
}

void Iocp::postReadEvent(const TcpConnectionPtr &conn, CpEvent *e)
{
    if (NULL == e) {
        conn->enterEventLoop();
        e = CpEventPool::instance()->get(this, conn);
    }
    e->setEventType(kEventTypeRead);
    e->setEventCallback(handleRead, handleError);
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

void Iocp::postWriteEvent(const TcpConnectionPtr &conn, CpEvent *e)
{
    size_t writeable = conn->writeBuffer()->readableBytes();
    if (writeable > 0) {
        int len = (writeable < CpEvent::kMaxBufSize) ? writeable : CpEvent::kMaxBufSize;
        
        if (NULL == e) {
            conn->enterEventLoop();
            e = CpEventPool::instance()->get(this, conn);
        }
        e->setEventType(kEventTypeWrite);
        e->setEventCallback(handleWrite, handleError);
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

void Iocp::postZeroByteReadEvent(const TcpConnectionPtr &conn, CpEvent *e)
{
    if (NULL == e) {
        conn->enterEventLoop();
        e = CpEventPool::instance()->get(this, conn);
    }
    e->setEventType(kEventTypeZeroByteRead);
    e->setEventCallback(handleZeroByteRead, handleError);
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

void Iocp::postCloseEvent(const TcpConnectionPtr &conn)
{
    conn->enterEventLoop();
    CpEvent *e = CpEventPool::instance()->get(this, conn);
    e->setEventType(kEventTypeClose);
    e->setEventCallback(handleError, handleError);
    _eventloop->postEvent(e);
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