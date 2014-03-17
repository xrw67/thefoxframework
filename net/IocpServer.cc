#include <net/IocpServer.h>
#include <net/Buffer.h>
#include <net/TcpConnection.h>

using namespace thefox;

namespace thefox
{
    
class IoContext
{
public:
    OVERLAPPED _overlapped;
    static const int kMaxBufSize = 8192;
    enum IoType{
        kRead,
        kWrite,
    };
        
    IoContext(void)
		: _bufUsed(0)
    {
		// OVERLAPPED需要初始化，否则WSARecv会ERROR_INVALID_HANDLE
		memset(&_overlapped, 0, sizeof(OVERLAPPED));

		// 初始化WSABUF
        _wsabuf.buf = static_cast<char *>(malloc(kMaxBufSize));
		memset(_wsabuf.buf, 0, kMaxBufSize);
        _wsabuf.len = kMaxBufSize;
    }
    ~IoContext(void)
    {
		free(_wsabuf.buf);
	}
    const IoType &getIoType() const { return _ioType; }
    void setIoType(IoType type) { _ioType = type; }
    int setBuffer(const char *data, size_t len)
    {
		memset(_wsabuf.buf, 0, kMaxBufSize);
		if (len > kMaxBufSize)
			len = kMaxBufSize;
		memcpy(_wsabuf.buf, data, len);
        _wsabuf.len = static_cast<u_long>(len);
		_bufUsed = len;
		return _wsabuf.len;
    }
    void resetBuffer()
    {
		memset(&_overlapped, 0, sizeof(OVERLAPPED));
		memset(_wsabuf.buf, 0, kMaxBufSize);
        _wsabuf.len = kMaxBufSize;
		_bufUsed = 0;
    }
    WSABUF &getWsaBuffer() { return _wsabuf; }
	void setBufUsed(size_t len) { _bufUsed = len; }
    size_t getBufferUsed() const { return _bufUsed; }
private:
    IoType _ioType;
    WSABUF _wsabuf;
	size_t _bufUsed;
};
        
// IOCP工作线程
DWORD WINAPI workerThreadProc(LPVOID param)
{
	IocpServer *server = reinterpret_cast<IocpServer *>(param);
	server->workerLoop();
	return 0;
}

// 接收新连接的线程
DWORD WINAPI acceptorThreadProc(LPVOID param)
{
	IocpServer *server = reinterpret_cast<IocpServer *>(param);
	server->acceptorLoop();
	return 0;
}

}


IocpServer::IocpServer(const String &nameArg, const InetAddress &listenAddr)
    : _name(nameArg)
	, _listenAddr(listenAddr)
	, _nextConnId(1)
	, _started(false)
{
}

IocpServer::~IocpServer()
{
    _started = false;
	PostQueuedCompletionStatus(_hIocp, 0, (ULONG_PTR)0, 0);

	 for (ConnectionMap::iterator it = _connections.begin(); it != _connections.end(); ++it) {
        TcpConnectionPtr conn = it->second;
        delete conn;
    }
}

bool IocpServer::initIocp()
{
    _hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (NULL == _hIocp) {
		int errCode = GetLastError();
		// LOG_ERROR << CreateIoCompletionPort failed! << errcode: << errCode;
		return false;
	}

	HANDLE handle;
	SYSTEM_INFO si;
    GetSystemInfo(&si);
    for (DWORD i = 0; i < (si.dwNumberOfProcessors * 2 + 2); ++i) {
		handle = CreateThread(NULL, 0, workerThreadProc, this, 0, NULL);
        CloseHandle(handle);
    }

	// LOG_INFO << initTocp done;
	return true;
}

bool IocpServer::start()
{
	if (started()) {
		// LOG_WARN << tcpserver already started;
		return false;
	}
	_started = true;

	if (!initIocp()) {
		// LOG_ERROR << initIocp failed;
		return false;
	}

    _listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET  == _listenSocket) {
		int errCode = WSAGetLastError();
		// LOG_ERROR << WSASocket failed!, errcode:<< errCode;
		return false;
	}
    _hAcceptEvent = WSACreateEvent();
    WSAEventSelect(_listenSocket, _hAcceptEvent, FD_ACCEPT);
    int ret = bind(_listenSocket, (struct sockaddr *)&_listenAddr.getSockAddrInet(), sizeof(struct sockaddr_in));
	if (SOCKET_ERROR   == ret) {
		int errCode = WSAGetLastError();
		// LOG_ERROR << WSASocket failed!, errcode:<< errCode;
		closesocket(_listenSocket);
		return false;
	}
    ret = listen(_listenSocket, 200);
	if (SOCKET_ERROR   == ret) {
		int errCode = WSAGetLastError();
		// LOG_ERROR << WSASocket failed!, errcode:<< errCode;
		closesocket(_listenSocket);
		return false;
	}
    HANDLE handle = CreateThread(NULL, 0, acceptorThreadProc, this, 0, NULL);
    CloseHandle(handle);

	// LOG_INFO << tcpserver start done;
	return true;
}

bool IocpServer::started()
{
	return _started;
}

void IocpServer::send(const TcpConnectionPtr &conn, const char *data, size_t len)
{
	conn->appendWriteBuffer(data, len);
	handleWrite(conn);
}

void IocpServer::removeConnection(const TcpConnectionPtr &conn)
{
	conn->setState(TcpConnection::kDisconnected);
	_closeCallback(conn);
	MutexLockGuard lock(_connLock);
	_connections.erase(conn->getConnId());
}

void IocpServer::setConnectionCallback(const ConnectionCallback &cb)
{
	_connectionCallback = cb;
}

void IocpServer::setCloseCallback(const CloseCallback &cb)
{
	_closeCallback = cb;
}

void IocpServer::setMessageCallback(const MessageCallback &cb)
{
	_messageCallback = cb;
}

void IocpServer::setWriteCompleteCallback(const WriteCompleteCallback &cb)
{
	_writeCompleteCallback = cb;
}

void IocpServer::newConnection(SOCKET socket, const InetAddress &peerAddr)
{
	int32_t connId = _nextConnId;
	++_nextConnId;

    TcpConnectionPtr conn = new TcpConnection(socket, connId, peerAddr);
    conn->setState(TcpConnection::kConnecting);
    _connections[connId] = conn;
	HANDLE handle = CreateIoCompletionPort((HANDLE)socket, _hIocp, (ULONG_PTR)conn, 0);
	if (NULL == handle) {
		int errCode = GetLastError();
		// LOG_ERROR 
	}
	IoContextPtr io = new IoContext();
	io->setIoType(IoContext::kRead);
	io->resetBuffer();
	DWORD nBytes = 0;
	DWORD flags = 0;
	int bytesRecv = WSARecv(conn->getSocket(), &io->getWsaBuffer(), 1, 
			&nBytes, &flags, &io->_overlapped, NULL);
	if (SOCKET_ERROR == bytesRecv) {
		int errCode = WSAGetLastError();
		if (WSA_IO_PENDING != errCode) {
			delete io;
			removeConnection(conn);
		}
	}

	conn->setState(TcpConnection::kConnected);
	_connectionCallback(conn);
}

void IocpServer::handleRead(const TcpConnectionPtr &conn, IoContextPtr io)
{
	if (0 == io->getBufferUsed()) {
		// LOG_INFO << connection closed！，recv 0 bytes data！
		delete io;
		removeConnection(conn);
		return;
	}

	conn->appendReadBuffer(io->getWsaBuffer().buf, io->getBufferUsed());
	io->resetBuffer();
	DWORD nBytes = 0;
	DWORD flags = 0;
	int bytesRecv = WSARecv(conn->getSocket(), &io->getWsaBuffer(), 1, 
			&nBytes, &flags, &io->_overlapped, NULL);
	if ((SOCKET_ERROR == bytesRecv) && (WSA_IO_PENDING != WSAGetLastError())) {
		delete io;
		removeConnection(conn);
	}
	_messageCallback(conn, conn->getReadBuffer(), Timestamp(Timestamp::now()));
}

void IocpServer::handleWrite(const TcpConnectionPtr &conn, IoContextPtr io)
{
	Buffer *readBuf = conn->getWriteBuffer();
	int bytesInReadBuffer = readBuf->readableBytes();
	if (bytesInReadBuffer > 0) {
		int len = (bytesInReadBuffer < IoContext::kMaxBufSize) ? bytesInReadBuffer : IoContext::kMaxBufSize;
		if (NULL == io)
			io = new IoContext();
		io->setIoType(IoContext::kWrite);
		io->setBuffer(readBuf->peek(), len);
		DWORD nBytes = 0;
		DWORD flags = 0;
		int byteSend = WSASend(conn->getSocket(), &io->getWsaBuffer(), 1, 
				&nBytes, flags, &io->_overlapped, NULL);
		if ((SOCKET_ERROR == byteSend) && (WSA_IO_PENDING != WSAGetLastError())) {
			delete io;
			removeConnection(conn);
		}
		readBuf->retrieve(len);
	} else {
		if (NULL != io)
			delete io;
	}
}

void IocpServer::workerLoop()
{
	DWORD bytesTransfered = 0;
    TcpConnectionPtr conn = NULL;
	OVERLAPPED *overlapped = NULL;
	BOOL ret = FALSE;
	
    while (started()) {
	    ret = GetQueuedCompletionStatus(_hIocp,
                                        &bytesTransfered,
                                        (LPDWORD)&conn, 
                                        &overlapped,
                                        INFINITE);
        if (!ret) {
            DWORD errCode = GetLastError();
            if (conn && WAIT_TIMEOUT != errCode)
				removeConnection(conn);
            if (overlapped) {
                IoContext *io = NULL;
		        if ((io = CONTAINING_RECORD(overlapped, IoContext, _overlapped)) != NULL)
                    delete io;
            }
            continue;
        }

        if (ret && conn && overlapped) {
            IoContext *io = NULL;
		    if ((io = CONTAINING_RECORD(overlapped, IoContext, _overlapped)) != NULL) {
				io->setBufUsed(bytesTransfered);
				if (IoContext::kRead == io->getIoType())
					handleRead(conn, io);
				else if (IoContext::kWrite == io->getIoType())
					handleWrite(conn, io);
			}
        }

        if (NULL == conn && NULL == overlapped)
            break;
    }
}

void IocpServer::acceptorLoop()
{
    while (started()) {
        if (WSA_WAIT_TIMEOUT != WSAWaitForMultipleEvents(1,&_hAcceptEvent, FALSE, 100, FALSE)) {
			WSANETWORKEVENTS events;
			if (SOCKET_ERROR != WSAEnumNetworkEvents(_listenSocket, _hAcceptEvent, &events)) {
				if (events.lNetworkEvents & FD_ACCEPT && 0 == events.iErrorCode[FD_ACCEPT_BIT]) {
					SOCKET clientSocket = INVALID_SOCKET;
					struct sockaddr_in addr;
					int len = sizeof(addr);
					clientSocket = WSAAccept(_listenSocket, (sockaddr *)&addr, &len, 0, 0);
					if (SOCKET_ERROR != clientSocket)
						newConnection(clientSocket, InetAddress(addr));
				}
			}
        }
    }
}