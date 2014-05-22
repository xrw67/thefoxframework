#include <net/iocp_event.h>
#include <log/logging.h>
#include <net/buffer.h>
#include <net/event.h>
#include <net/tcp_connection.h>

using namespace thefox;

IocpEvent::IocpEvent()
	: _hIocp(NULL)
{
}

IocpEvent::~IocpEvent()
{
	if (NULL != _hIocp) {
		if (0 == ::CloseHandle(_hIocp)) {
			DWORD err = ::GetLastError();
			THEFOX_LOG(ERROR) << "iocp CloseHandle() failed, errno=" << err;
		}
	}
	_hIocp = NULL;
}

bool IocpEvent::init()
{
	if (NULL != _hIocp)
		_hIocp = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	if (NULL == _hIocp) {
		DWORD err = ::GetLastError();
		THEFOX_LOG(ERROR) << "CreateIoCompletionPort() failed, errno=" << err;
	}
	return (NULL == _hIocp);
}

bool IocpEvent::addEvent(IoEvent *ev)
{
	if (NULL == CreateIoCompletionPort((HANDLE)ev->conn, _hIocp, 0, 0)) {
		DWORD err = ::GetLastError();
		THEFOX_LOG(ERROR) << "addEvent() failed, errno=" << err;
		return false;
	}
	return true;
}

bool IocpEvent::delConnection(TcpConnection *conn)
{
	if (0 == ::CancelIo((HANDLE)conn->fd())) {
		THEFOX_LOG(ERROR) << "CancelIo() failed";
		return false;
	}
	return true;
}

bool IocpEvent::processEvents(uint32_t timer)
{
	DWORD err = 0;
	DWORD bytes = 0;
    ULONG_PTR key = NULL;
	IoEvent *ev = NULL;
    BOOL ret = FALSE;
    ret = ::GetQueuedCompletionStatus(_hIocp, &bytes, &key, 
									(LPOVERLAPPED *)&ev, (DWORD)timer);
    
	if (0 == bytes && NULL == key && NULL == ev) {
		THEFOX_LOG(INFO) << "GetQueuedCompletionStatus() quit";
        return false;
	}

	if (0 == ret)
		err = ::GetLastError();
	else
		err = 0;

	if (err) {
        if (NULL == ev) {
			if (WAIT_TIMEOUT != ::GetLastError()) {
				THEFOX_LOG(ERROR) << "GetQueuedCompletionStatus() failed";
				return false;
			}
			return true;
        }
    }

	if (NULL == ev) {
		THEFOX_LOG(WARN) << "GetQueuedCompletionStatus() returned no operation";
		return true;
	}

	if (err == ERROR_NETNAME_DELETED /* the socket was closed */
            || err == ERROR_OPERATION_ABORTED /* the operation was canceled */) {
        /*
         * the WSA_OPERATION_ABORTED completion notification
         * for a file descriptor that was closed
         */

        THEFOX_LOG(WARN) << "iocp: aborted event: " << ev << ", err:" << err;
        return true;
    }

	if (err)
		THEFOX_LOG(ERROR) << "GetQueuedCompletionStatus() returned operation error:" << err;

	ev->avaliable = bytes;

	handler(ev);

	return true;
}

bool IocpEvent::postRead(IoEvent *ev)
{
	ev->enterIo();

	WSABUF wsabuf[1];
	TcpConnection *conn = ev->conn;
	Buffer *readbuf = conn->readBuffer();

	// 设置读缓冲区大小
	readbuf->ensureWritableBytes(kMaxBufSize);

	wsabuf[0].buf = readbuf->beginWrite();
	wsabuf[0].len = readbuf->writableBytes();

	DWORD nBytes = 0;
    DWORD flags = 0;
    int bytesRecv = WSARecv(conn->fd(), wsabuf, 1, 
                            &nBytes, &flags, &ev->ovlp, NULL);
    if (SOCKET_ERROR == bytesRecv && WSA_IO_PENDING != WSAGetLastError()) {
        THEFOX_LOG(ERROR) << "postRead() failed!";
		//  关闭connection
		ev->leaveIo();
		conn->connectDestroyed();
		return false;
    }
	return true;
}

bool IocpEvent::postWrite(IoEvent *ev)
{
	ev->enterIo();

	WSABUF wsabuf[1];
	TcpConnection *conn = ev->conn;
	Buffer *writebuf = conn->writeBuffer();

	wsabuf[0].buf = writebuf->peek();
	wsabuf[0].len = writebuf->readableBytes();

	DWORD nBytes = 0;
    DWORD flags = 0;
    int byteSend = WSARecv(conn->fd(), wsabuf, 1, 
                            &nBytes, &flags, &ev->ovlp, NULL);
    if (SOCKET_ERROR == byteSend && WSA_IO_PENDING != WSAGetLastError()) {
        THEFOX_LOG(ERROR) << "postWrite() failed!";
		//  关闭connection
		ev->leaveIo();
		conn->connectDestroyed();
		return false;
    }
	return true;
}

void IocpEvent::handler(IoEvent *ev)
{
	if (ev->avaliable > 0) {
		if (ev->read)
			handleRead(ev);
		if (ev->write)
			handleWrite(ev);
	}

	if (0 == ev->avaliable || ev->close) {
		ev->leaveIo();
		TcpConnection *conn = ev->conn;
		conn->connectDestroyed();
	}
}
void IocpEvent::handleRead(IoEvent *ev)
{
	TcpConnection *conn = ev->conn;
	Buffer *readbuf = conn->readBuffer();
	
	readbuf->hasWritten(ev->avaliable);
	
	ev->avaliable = 0;
	ev->leaveIo();
	
	postRead(ev);
}

void IocpEvent::handleWrite(IoEvent *ev)
{
	TcpConnection *conn = ev->conn;
	Buffer *writebuf = conn->writeBuffer();

	writebuf->retrieve(ev->avaliable);
	
	ev->avaliable = 0;
	ev->leaveIo();

	if (0 == writebuf->readableBytes())
		ev->write = false;
	else
		postWrite(ev);
}

