#include <net/iocp_event.h>
#include <log/logging.h>
#include <net/buffer.h>
#include <net/tcp_connection.h>

using namespace thefox;

IocpEvent::IocpEvent()
	: _hIocp(NULL)
{
	THEFOX_TRACE_FUNCTION;
}

IocpEvent::~IocpEvent()
{
	THEFOX_TRACE_FUNCTION;

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
	THEFOX_TRACE_FUNCTION;

	if (NULL == _hIocp)
		_hIocp = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	if (NULL == _hIocp) {
		DWORD err = ::GetLastError();
		THEFOX_LOG(ERROR) << "CreateIoCompletionPort() failed, errno=" << err;
	}
	return (NULL == _hIocp);
}

bool IocpEvent::addEvent(IoEvent *ev)
{
	THEFOX_TRACE_FUNCTION << "event=" << ev;

	TcpConnection *conn = ev->conn;
	if (NULL == CreateIoCompletionPort((HANDLE)conn->fd(), _hIocp, 0, 0)) {
		DWORD err = ::GetLastError();
		THEFOX_LOG(ERROR) << "addEvent() failed, errno=" << err;
		return false;
	}
	return true;
}
bool IocpEvent::postClose(IoEvent *ev)
{
	THEFOX_TRACE_FUNCTION << "event=" << ev;

	TcpConnection *conn = ev->conn;

	ev->enterIo();

	EventOvlp *ovlp = _ovlpPool.get();
	::ZeroMemory(ovlp, sizeof(EventOvlp));
	ovlp->type = OVLP_TYPE_CLOSE;
	ovlp->ev = ev;

	if (0 == ::PostQueuedCompletionStatus(_hIocp, 0, 
			NULL, &ovlp->ovlp)) {
		DWORD err = GetLastError();
		THEFOX_LOG(ERROR) << "postEvent() failed, errno=" << err;
		ev->leaveIo();
		_ovlpPool.put(ovlp);
		conn->connectDestroyed();
		return false;
	}
	return true;
}

bool IocpEvent::delConnection(TcpConnection *conn)
{
	THEFOX_TRACE_FUNCTION << "conn_id" << conn->id();

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
	EventOvlp *ovlp = NULL;
    BOOL ret = FALSE;
    ret = ::GetQueuedCompletionStatus(_hIocp, &bytes, &key, 
									(LPOVERLAPPED *)&ovlp, (DWORD)timer);
	if (0 == ret)
		err = ::GetLastError();
	else
		err = 0;

	if (err) {
        if (NULL == ovlp) {
			if (WAIT_TIMEOUT != ::GetLastError()) {
				THEFOX_LOG(ERROR) << "GetQueuedCompletionStatus() failed";
				return false;
			}
			return true;
        }
    }

	if (NULL == ovlp) {
		THEFOX_LOG(WARN) << "GetQueuedCompletionStatus() returned no operation";
		return true;
	}

	int32_t ovlpType = ovlp->type;
	IoEvent *ev = ovlp->ev;

	_ovlpPool.put(ovlp);

	if (NULL == ev) {
		THEFOX_LOG(WARN) << "iocp: error evvnt, err:" << err;
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

	handler(ev, ovlpType, bytes);

	return true;
}

bool IocpEvent::updateRead(IoEvent *ev)
{
	THEFOX_TRACE_FUNCTION << "event=" << ev;

	ev->enterIo();

	WSABUF wsabuf[1];
	TcpConnection *conn = ev->conn;
	Buffer *readbuf = conn->readBuffer();

	// 设置读缓冲区大小
	readbuf->ensureWritableBytes(kMaxBufSize);

	wsabuf[0].buf = readbuf->beginWrite();
	wsabuf[0].len = readbuf->writableBytes();

	EventOvlp *ovlp = _ovlpPool.get();
	::ZeroMemory(ovlp, sizeof(EventOvlp));
	ovlp->type = OVLP_TYPE_READ;
	ovlp->ev = ev;

	DWORD nBytes = 0;
    DWORD flags = 0;
    int bytesRecv = WSARecv(conn->fd(), wsabuf, 1, 
							&nBytes, &flags, &ovlp->ovlp, NULL);
    if (SOCKET_ERROR == bytesRecv && WSA_IO_PENDING != WSAGetLastError()) {
        THEFOX_LOG(ERROR) << "postRead() failed!";
		//  关闭connection
		ev->leaveIo();
		_ovlpPool.put(ovlp);
		conn->connectDestroyed();
		return false;
    }
	return true;
}

bool IocpEvent::updateWrite(IoEvent *ev)
{
	THEFOX_TRACE_FUNCTION << "event=" << ev;

	ev->enterIo();

	WSABUF wsabuf[1];
	TcpConnection *conn = ev->conn;
	Buffer *writebuf = conn->writeBuffer();

	wsabuf[0].buf = writebuf->peek();
	wsabuf[0].len = writebuf->readableBytes();

	EventOvlp *ovlp = _ovlpPool.get();
	::ZeroMemory(ovlp, sizeof(EventOvlp));
	ovlp->type = OVLP_TYPE_WRITE;
	ovlp->ev = ev;

	DWORD nBytes = 0;
    DWORD flags = 0;
    int byteSend = WSASend(conn->fd(), wsabuf, 1, 
                            &nBytes, flags, &ovlp->ovlp, NULL);
    if (SOCKET_ERROR == byteSend && WSA_IO_PENDING != WSAGetLastError()) {
        THEFOX_LOG(ERROR) << "postWrite() failed!";
		//  关闭connection
		ev->leaveIo();
		_ovlpPool.put(ovlp);
		conn->connectDestroyed();
		return false;
    }
	return true;
}

void IocpEvent::handler(IoEvent *ev, int32_t ovlpType, uint32_t avaliable)
{
	THEFOX_TRACE_FUNCTION << "type=" << ovlpType << " avaliable=" << avaliable;

	if (0 == avaliable || ev->close) {
		ev->leaveIo();
		TcpConnection *conn = ev->conn;
		conn->connectDestroyed();
		return;
	}

	switch (ovlpType) {
	case OVLP_TYPE_READ:
		handleRead(ev, avaliable);
		break;
	case OVLP_TYPE_WRITE:
		handleWrite(ev, avaliable);
		break;
	default:
		break;
	}
}
void IocpEvent::handleRead(IoEvent *ev, uint32_t avaliable)
{
	THEFOX_TRACE_FUNCTION << "event=" << ev << " avaliable=" << avaliable;

	TcpConnection *conn = ev->conn;
	Buffer *readbuf = conn->readBuffer();
	
	readbuf->hasWritten(avaliable);
	
	conn->addReadBytes(avaliable);

	if (conn->_messageCallback)
		conn->_messageCallback(conn, readbuf, Timestamp::now());

	ev->leaveIo();
	
	updateRead(ev);
}

void IocpEvent::handleWrite(IoEvent *ev, uint32_t avaliable)
{
	THEFOX_TRACE_FUNCTION << "event=" << ev << " avaliable=" << avaliable;

	TcpConnection *conn = ev->conn;
	Buffer *writebuf = conn->writeBuffer();

	writebuf->retrieve(avaliable);
	
	conn->addWriteBytes(avaliable);

	if (conn->_writeCompleteCallback)
		conn->_writeCompleteCallback(conn);

	ev->leaveIo();

	if (0 == writebuf->readableBytes())
		ev->write = false;
	else
		updateWrite(ev);
}
