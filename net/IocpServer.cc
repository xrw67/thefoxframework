#include <net/IocpServer.h>
#include <net/Buffer.h>

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
    {
        _wsa.buf = _buffer;
        _wsa.len = sizeof(_buffer);
    }
    ~IoContext(void)
    {}
    const IoType &getIoType() const { return _ioType; }
    void setIoType(IoType type) { _ioType = type; }
    void setBuffer(const char *data, size_t len)
    {
        memcpy(_buffer, data, len);
        _wsa.len = static_cast<u_long>(len);
    }
    void resetBuffer()
    {
        _wsa.buf = _buffer;
        _wsa.len = sizeof(_buffer);
    }
    WSABUF &getWsaBuf() { return _wsa; }
    int getBufUsed() const { return _wsa.len; }
private:
    IoType _ioType;
    WSABUF _wsa;
    char _buffer[kMaxBufSize];
};
    
class Connection
{
public:
	enum StateT { kDisconnected, kConnecting, kConnected, kDisconnecting };

	Connection(SOCKET socket, int connId, const InetAddress &peerAddr)
		: _socket(socket)
		, _connId(connId)
		, _peerAddr(peerAddr)
	{}
	~Connection()
	{
		if (INVALID_SOCKET != _socket) {
			closesocket(_socket);
			_socket = INVALID_SOCKET;
		}
	}
	int getConnId() const { return _connId; }
	SOCKET getSocket() { return _socket; }
	const InetAddress &getPeerAddr() const { return _peerAddr; }
	Buffer *getReadBuffer() { return &_readBuffer; }
	Buffer *getWriteBuffer() { return &_writeBuffer; }
	void appendReadBuffer(IoContextPtr io) 
	{
		if (io)
			_readBuffer.append(io->getWsaBuf().buf, io->getBufUsed());
	}
	void appendWriteBuffer(const char *data, size_t len)
	{
		_writeBuffer.append(data, len);
	}
	void setState(StateT state) { _state = state; }
private:
	int _connId;
	SOCKET _socket;
	InetAddress _peerAddr;
	Buffer _readBuffer;
	Buffer _writeBuffer;
	StateT _state;
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
        ConnectionPtr conn = it->second;
        delete conn;
    }
}

bool IocpServer::start()
{
	if (started())
		return false;
	_started = true;

	HANDLE handle;
 
    _hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	SYSTEM_INFO si;
    GetSystemInfo(&si);
    for (DWORD i = 0; i < (si.dwNumberOfProcessors * 2 + 2); ++i) {
		handle = CreateThread(NULL, 0, workerThreadProc, this, 0, NULL);
        CloseHandle(handle);
    }

    _listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    _hAcceptEvent = WSACreateEvent();
    WSAEventSelect(_listenSocket, _hAcceptEvent, FD_ACCEPT);
    bind(_listenSocket, (struct sockaddr *)&_listenAddr.getSockAddrInet(), sizeof(struct sockaddr_in));
    listen(_listenSocket, 5);
    handle = CreateThread(NULL, 0, acceptorThreadProc, this, 0, NULL);
    CloseHandle(handle);
	return true;
}

bool IocpServer::started()
{
	return _started;
}

void IocpServer::send(int32_t connId, const char *data, size_t len)
{
	ConnectionPtr conn = NULL;
	if ((conn = getConnectionById(connId)) != NULL) {
		conn->appendWriteBuffer(data, len);
		handleWrite(conn);
	}
}

void IocpServer::removeConnection(int32_t connId)
{
	MutexLockGuard lock(_connLock);
	ConnectionPtr conn = getConnectionById(connId);
	if (NULL != conn)
		removeConnection(conn);
	else
		;// LOG_WARN << 移除一个不存在的连接
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

    ConnectionPtr conn = new Connection(socket, connId, peerAddr);
    conn->setState(Connection::kConnected);
    _connections[connId] = conn;
	CreateIoCompletionPort((HANDLE)socket, _hIocp, (ULONG_PTR)conn, 0);
	IoContextPtr io = new IoContext();
	io->setIoType(IoContext::kRead);
	io->resetBuffer();
	DWORD bytesRecv = 0;
	DWORD flags = 0;
	WSARecv(conn->getSocket(), 
			&io->getWsaBuf(), 
			1, 
			&bytesRecv, 
			&flags, 
			&io->_overlapped, 
			NULL);
	_connectionCallback(connId);
}

void IocpServer::removeConnection(ConnectionPtr conn)
{
	delete conn;
	// LOG_INFO <<　移除一个用户
}

void IocpServer::handleRead(ConnectionPtr conn, IoContextPtr io)
{
	conn->appendReadBuffer(io);
	io->resetBuffer();
	DWORD bytesRecv = 0;
	DWORD flags = 0;
	WSARecv(conn->getSocket(), 
			&io->getWsaBuf(), 
			1, 
			&bytesRecv, 
			&flags, 
			&io->_overlapped, 
			NULL);
}

void IocpServer::handleWrite(ConnectionPtr conn, IoContextPtr io)
{
	Buffer *readBuf = conn->getWriteBuffer();
	int bytesInReadBuffer = readBuf->readableBytes();
	if (bytesInReadBuffer > 0) {
		int len = (bytesInReadBuffer < IoContext::kMaxBufSize) ? bytesInReadBuffer : IoContext::kMaxBufSize;
		if (NULL == io)
			io = new IoContext();
		io->setIoType(IoContext::kWrite);
		io->setBuffer(readBuf->peek(), len);
		DWORD bytesSend = 0;
		DWORD flags = 0;
		WSASend(conn->getSocket(), 
				&io->getWsaBuf(), 
				1, 
				&bytesSend, 
				flags, 
				&io->_overlapped, 
				NULL);
		readBuf->retrieve(len);
	}
}

ConnectionPtr IocpServer::getConnectionById(int32_t connId)
{
	ConnectionMap::iterator it = _connections.find(connId);
	if (it != _connections.find(connId))
		return it->second;
	else
		return NULL;
}

void IocpServer::workerLoop()
{
	DWORD bytesTransfered = 0;
    ConnectionPtr conn = NULL;
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
                _closeCallback(conn->getConnId());
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