#pragma warning(disable:4996) // 禁用warning 4996

#include <net/iocp.h>
#include <net/buffer.h>
#include <net/cp_event.h>
#include <net/event_loop.h>
#include <net/tcp_connection.h>

namespace thefox
{



using namespace thefox;


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
        
        MutexGuard lock(_connMutex);
        _connections.erase(conn->connId());
	}
}

void Iocp::postReadEvent(const TcpConnectionPtr &conn, CpEvent *e)
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

void Iocp::postWriteEvent(const TcpConnectionPtr &conn, CpEvent *e)
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

void Iocp::postZeroByteReadEvent(const TcpConnectionPtr &conn, CpEvent *e)
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

void Iocp::postCloseEvent(const TcpConnectionPtr &conn)
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
    TcpConnectionPtr conn = se->conn();

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