#include <net/TcpServer.h>
#include <net/TcpConnection.h>
#include <net/IoCompletionPort.h>
#include <net/Socket.h>

using namespace thefox;

TcpServer::TcpServer(const InetAddress &listenAddr, const String &nameArg)
	: _hostport(listenAddr.toIpPort())
	, _name(nameArg)
	, _acceptor(listenAddr)
	, _nextConnId(1)
{
	_iocp.registerHandle((HANDLE)_acceptor._acceptSocket, 0);
}

TcpServer::~TcpServer()
{
	for (ConnectionMap::iterator it(_connections.begin()); it != _connections.end(); ++it) {
		TcpConnectionPtr conn = it->second;
		delete conn;
	}
}

void TcpServer::start()
{
	if (_started)
		return;
	
	_started = true;
	
	// 创建工作线程
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	int threadNumber = 2 * si.dwNumberOfProcessors + 2;
	for (int i = 0; i < threadNumber; ++i)
		::CreateThread(NULL, 0, IoWorkThread, this, 0, 0);
	
	// 监听线程
	int &defaultPostAcceptNum = threadNumber;
	while (_acceptor._IoBuffers.size() < defaultPostAcceptNum) {
		IoBuffer *buf = _ioBufferPool.getBuffer(OpAccept);
		if (_acceptor.postAccept(buf))
			_acceptor.insertPengingIoBuffer(buf);
		else
			delete buf;
	}
	::WSAEventSelect(_acceptor._acceptSocket, _acceptor._acceptEvent, FD_ACCEPT);
	::CreateThread(NULL, 0, AcceptThread, this, 0, 0);
}

void TcpServer::OnNewConnection(SOCKET socket, const InetAddress &localAddr, const InetAddress &peerAddr)
{
	char buf[32];
	String connName;
	{
		MutexLockGuard lock(_connectionMapLock);
		_snprintf(buf, sizeof(buf), ":%s#%d", _hostport.c_str(), _nextConnId);
		++_nextConnId;
		connName = _name + buf;
	}
	
	setTcpNoDelay(socket, true); // 禁用nagle算法
	TcpConnectionPtr conn(new TcpConnection(connName, socket, localAddr, peerAddr));
	_connections[connName] = conn;
	_iocp.registerHandle((HANDLE)socket, (ULONG_PTR)conn);
	postRecv(conn->_socket, _ioBufferPool.getBuffer(OpRead));
}


void TcpServer::OnConnectionClose(const TcpConnectionPtr &conn)
{

}

void TcpServer::OnConnectionError(const TcpConnectionPtr &conn)
{

}

void TcpServer::OnReadCompleted(const TcpConnectionPtr &conn, Buffer *buf, Timestamp ts)
{

}

void TcpServer::OnWriteCompleted(const TcpConnectionPtr &conn)
{

}

void TcpServer::removeConnection(const TcpConnectionPtr &conn)
{

}

void TcpServer::IoProcess(const TcpConnectionPtr &conn, IoBuffer *buf)
{
	switch (buf->getIoOperation()) {
		case OpAccept: {
			InetAddress localAddr;
			InetAddress peerAddr;
			_acceptor.getAcceptExSockAddrs(buf, localAddr, peerAddr);
			OnNewConnection(buf->_socket, localAddr, peerAddr);
			buf->resetBuffer();
			_acceptor.postAccept(buf);
			break;						  
		}
		case OpReadCompleted: {
			conn->_inBuffer.readIoBuffer(buf);
			OnReadCompleted(conn, &conn->_inBuffer, Timestamp::now());
			buf->resetBuffer();
			postRecv(conn->_socket, buf);
			break;
		}
		case OpWriteCompleted: {
				
			break;
		}
	}
}
DWORD TcpServer::AcceptThread(LPVOID serverPtr)
{
	TcpServer *svr = reinterpret_cast<TcpServer *>(serverPtr);
	Acceptor &acceptor = svr->_acceptor;
	
	HANDLE events[] = {acceptor._acceptEvent, acceptor._stopEvent };

	while (svr->_started) {
		DWORD dwResult = ::WSAWaitForMultipleEvents(2, events, FALSE, INFINITE, FALSE);
		if(dwResult == WAIT_OBJECT_0) {
			IoBuffer *buf = svr->_ioBufferPool.getBuffer(OpAccept);
			if (acceptor.postAccept(buf))
				acceptor.insertPengingIoBuffer(buf);
			else
				delete buf;
		} else if(dwResult == WAIT_OBJECT_0 + 1) {
			//acceptor.releaseAcceptSockets();
			break;
		}
	}
	return 0;
}

DWORD TcpServer::IoWorkThread(LPVOID serverPtr)
{
	TcpServer *svr = reinterpret_cast<TcpServer *>(serverPtr);

	LPOVERLAPPED lpOverlapped = NULL;
	TcpConnectionPtr conn = NULL;
	IoBuffer *buf = NULL;
	DWORD bytesTransfered = 0;
	
	while (svr->_started) {
		lpOverlapped = NULL;
		conn = NULL;
		BOOL retCode = svr->_iocp.getStatus(&bytesTransfered, (PULONG_PTR)&conn, &lpOverlapped, INFINITE);
		
		if (!retCode) {
			DWORD errCode = ::GetLastError();
			if (WAIT_TIMEOUT != errCode && NULL != conn)
				svr->removeConnection(conn);
			continue;
		}
		
		if (retCode && conn && lpOverlapped) {
			buf = CONTAINING_RECORD(lpOverlapped, IoBuffer, _overlapped);
			if (buf)
				svr->IoProcess(conn, buf);
		}
			
		// 收到退出标志，直接退出
		if (NULL == conn &&  NULL == lpOverlapped)
			svr->_started = false;
	}
	return 0;
}