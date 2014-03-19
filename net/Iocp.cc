#include <net/Iocp.h>
#include <net/Buffer.h>
#include <net/TcpConnection.h>

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
		kZeroByteRead
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
	void setZeroByteBuffer()
	{
		memset(&_overlapped, 0, sizeof(OVERLAPPED));
		memset(_wsabuf.buf, 0, kMaxBufSize);
		_wsabuf.len = 0;
		_bufUsed = 0;
	}

    WSABUF &getWsaBuffer() { return _wsabuf; }
	void setBufferUsed(size_t len) { _bufUsed = len; }
    size_t getBufferUsed() const { return _bufUsed; }
private:
    IoType _ioType;
    WSABUF _wsabuf;
	size_t _bufUsed;
};
        
// IOCP工作线程
DWORD WINAPI workerThreadProc(LPVOID param)
{
	Iocp *server = reinterpret_cast<Iocp *>(param);
	server->workerLoop();
	return 0;
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

Iocp::Iocp(const String &nameArg)
    : _name(nameArg)
	, _nextConnId(1)
	, _started(false)
{
}

Iocp::~Iocp()
{
	close();
	CloseHandle(_hIocp);
}

int Iocp::getCpuNum()
{
	SYSTEM_INFO si;
    GetSystemInfo(&si);
	return si.dwNumberOfProcessors;
}

bool Iocp::initIocp()
{
    _hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (NULL == _hIocp) {
		int errCode = GetLastError();
		// LOG_ERROR << CreateIoCompletionPort failed! << errcode: << errCode;
		return false;
	}

	HANDLE handle;
	
	int threadNum = getCpuNum() * 2 + 2;
    for (int i = 0; i < threadNum; ++i) {
		handle = CreateThread(NULL, 0, workerThreadProc, this, 0, NULL);
        CloseHandle(handle);
    }

	// LOG_INFO << initTocp done;
	return true;
}

bool Iocp::start(const InetAddress &listenAddr)
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

    _socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET  == _socket) {
		int errCode = WSAGetLastError();
		// LOG_ERROR << WSASocket failed!, errcode:<< errCode;
		return false;
	}
    _hAcceptEvent = WSACreateEvent();
    WSAEventSelect(_socket, _hAcceptEvent, FD_ACCEPT);
    int ret = bind(_socket, (struct sockaddr *)&listenAddr.getSockAddrInet(), sizeof(struct sockaddr_in));
	if (SOCKET_ERROR   == ret) {
		int errCode = WSAGetLastError();
		// LOG_ERROR << WSASocket failed!, errcode:<< errCode;
		closesocket(_socket);
		return false;
	}
    ret = listen(_socket, 200);
	if (SOCKET_ERROR   == ret) {
		int errCode = WSAGetLastError();
		// LOG_ERROR << WSASocket failed!, errcode:<< errCode;
		closesocket(_socket);
		return false;
	}
    HANDLE handle = CreateThread(NULL, 0, acceptorThreadProc, this, 0, NULL);
    CloseHandle(handle);

	// LOG_INFO << tcpserver start done;
	return true;
}

void Iocp::send(int32_t connId, const char *data, size_t len)
{
	TcpConnection *conn = _connections[connId];
	conn->appendWriteBuffer(data, len);
	handleWrite(conn);
}

void Iocp::removeConnection(int32_t connId)
{
	TcpConnection *conn = _connections[connId];
	removeConnection(conn);
}

void Iocp::removeConnection(const TcpConnectionPtr &conn)
{
	conn->setState(TcpConnection::kDisconnected);
	_closeCallback(conn->getConnId());
	MutexLockGuard lock(_connLock);
	_connections.erase(conn->getConnId());
	delete conn;
}

void Iocp::setConnectionCallback(const ConnectionCallback &cb)
{
	_connectionCallback = cb;
}

void Iocp::setCloseCallback(const CloseCallback &cb)
{
	_closeCallback = cb;
}

void Iocp::setMessageCallback(const MessageCallback &cb)
{
	_messageCallback = cb;
}

void Iocp::setWriteCompleteCallback(const WriteCompleteCallback &cb)
{
	_writeCompleteCallback = cb;
}

void Iocp::newConnection(SOCKET socket, const InetAddress &peerAddr)
{
	int32_t connId = _nextConnId;
	++_nextConnId;

    TcpConnectionPtr conn = new TcpConnection(socket, connId, peerAddr);
    conn->setState(TcpConnection::kConnecting);
    _connections[connId] = conn;
	HANDLE handle = CreateIoCompletionPort((HANDLE)socket, _hIocp, (ULONG_PTR)conn, 0);
	if (handle == _hIocp) {
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
			return;
		}
	}

	handleZeroByteRead(conn, NULL);
	conn->setState(TcpConnection::kConnected);
	_connectionCallback(conn->getConnId());
}

void Iocp::handleRead(const TcpConnectionPtr &conn, IoContextPtr io)
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
	_messageCallback(conn->getConnId(), conn->getReadBuffer(), Timestamp(Timestamp::now()));
}

void Iocp::handleWrite(const TcpConnectionPtr &conn, IoContextPtr io)
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

void Iocp::handleZeroByteRead(const TcpConnectionPtr &conn, IoContextPtr io)
{
	if (NULL == io)
		io = new IoContext;
	io->setIoType(IoContext::kZeroByteRead);
	io->setZeroByteBuffer();
	DWORD nBytes = 0;
	DWORD flags = 0;
	int byteRecv = WSARecv(conn->getSocket(), &io->getWsaBuffer(), 1, 
			&nBytes, &flags, &io->_overlapped, NULL);
	if ((SOCKET_ERROR == byteRecv) && (WSA_IO_PENDING != WSAGetLastError())) {
		delete io;
		removeConnection(conn);
	}
}

bool Iocp::open(const InetAddress &serverAddr)
{
	if (started()) {
		// LOG_WARN << tcpserver already started;
		return false;
	}
	_started = true;

	if (!initIocp()) {
		// LOG_ERROR 
		return false;
	}

	_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET  == _socket) {
		int errCode = WSAGetLastError();
		// LOG_ERROR << WSASocket failed!, errcode:<< errCode;
		return false;
	}

	int ret = connect(_socket, 
					  (struct sockaddr *)&serverAddr.getSockAddrInet(), 
					  sizeof(struct sockaddr_in));
	if (SOCKET_ERROR == ret && WSAEWOULDBLOCK != WSAGetLastError()) {
		// LOG_ERROR 
		closesocket(_socket);
		return false;
	}
	newConnection(_socket, serverAddr);
	return true;
}

void Iocp::close()
{	
	_started = false;
	int threadNum = getCpuNum() * 2 + 2;
	for (int i = 0; i < threadNum; ++i)
		PostQueuedCompletionStatus(_hIocp, 0, (ULONG_PTR)0, 0);

	MutexLockGuard lock(_connLock);
	for (ConnectionMap::iterator it = _connections.begin(); it != _connections.end(); ++it) {
        TcpConnectionPtr conn = it->second;
        delete conn;
    }
	_connections.clear();
}

void Iocp::send(const char *data, size_t len)
{
	ConnectionMap::iterator it = _connections.begin();
	if (it != _connections.end())
		send(it->second->getConnId(), data, len);
}

void Iocp::workerLoop()
{
	DWORD bytesTransfered = 0;
    TcpConnectionPtr conn = NULL;
	OVERLAPPED *overlapped = NULL;
	BOOL ret = FALSE;
	
    while (started()) {
	    ret = GetQueuedCompletionStatus(
				_hIocp, &bytesTransfered, (LPDWORD)&conn, &overlapped, INFINITE);
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
				io->setBufferUsed(bytesTransfered);
				if (IoContext::kRead == io->getIoType())
					handleRead(conn, io);
				else if (IoContext::kWrite == io->getIoType())
					handleWrite(conn, io);
				else if (IoContext::kZeroByteRead == io->getIoType())
					handleZeroByteRead(conn, io);
			}
        }

        if (NULL == conn && NULL == overlapped)
            break;
    }
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