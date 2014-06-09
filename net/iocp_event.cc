#include <net/iocp_event.h>
#include <log/logging.h>
#include <net/buffer.h>
#include <net/tcp_connection.h>

using namespace thefox;
using namespace thefox::net;

IocpEvent::IocpEvent()
	: _hIocp(NULL)
{}

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

bool IocpEvent::addEvent(Event *ev)
{
	THEFOX_TRACE_FUNCTION;

	assert(ev->fd >= 0);

	ev->handler = std::bind(&IocpEvent::handler, this, _1, _2);

	if (NULL == ::CreateIoCompletionPort((HANDLE)ev->fd, _hIocp, 0, 0)) {
		DWORD err = ::GetLastError();
		THEFOX_LOG(ERROR) << "addEvent() failed, errno=" << err;
		return false;
	}

	return true;
}

bool IocpEvent::delEvent(Event *ev)
{
	THEFOX_TRACE_FUNCTION;

	if (0 == ::CancelIo((HANDLE)ev->fd)) {
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

    ovlp->avaliable = bytes;
	Event *ev = ovlp->ev;

	if (NULL == ev) {
		THEFOX_LOG(WARN) << "iocp: error evvnt, err:" << err;
        return true;
	}

	//if (err == ERROR_NETNAME_DELETED /* the socket was closed */
 //           || err == ERROR_OPERATION_ABORTED /* the operation was canceled */) {
 //       /*
 //        * the WSA_OPERATION_ABORTED completion notification
 //        * for a file descriptor that was closed
 //        */

 //       THEFOX_LOG(WARN) << "iocp: aborted event, type: " <<  ovlpType 
 //       				<< ", fd: "<< conn->fd() << ", err:" << err;
 //       return true;
 //   }

	if (err)
		THEFOX_LOG(ERROR) << "GetQueuedCompletionStatus() returned operation error:" << err;

    ev->handler(ev, (void *)ovlp);

    _ovlpPool.put(ovlp);

	return true;
}

bool IocpEvent::updateRead(Event *ev)
{
	THEFOX_TRACE_FUNCTION;

	TcpConnection *conn = ev->conn;

    if (NULL == conn)
        return false;

    conn->enterEvent();
    ev->in = true;

	WSABUF wsabuf[1];
	Buffer *readbuf = ev->conn->readBuffer();

	// set writable bytes
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
        THEFOX_LOG(ERROR) << "updateRead() failed!";
		// close connection
		_ovlpPool.put(ovlp);
		conn->connectDestroyed();
		return false;
    }
	return true;
}

bool IocpEvent::updateWrite(Event *ev)
{
	THEFOX_TRACE_FUNCTION;

	TcpConnection *conn = ev->conn;

    if (NULL == conn)
        return false;

    conn->enterEvent();

	WSABUF wsabuf[1];
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
        THEFOX_LOG(ERROR) << "updateWrite() failed!";
		// close connection
		_ovlpPool.put(ovlp);
		conn->connectDestroyed();
		return false;
    }
	return true;
}

void IocpEvent::handler(Event *ev, void *ovlp)
{
    THEFOX_TRACE_FUNCTION;
   
    EventOvlp *evol = (EventOvlp *)ovlp;
    int32_t type = evol->type;
    uint32_t avaliable = evol->avaliable;
    TcpConnection *conn = ev->conn;

    if (0 == avaliable || true == ev->error) {
		conn->connectDestroyed();
		return;
	}

    switch (type) {
	case OVLP_TYPE_READ:
        onRead(ev, avaliable);
		break;
	case OVLP_TYPE_WRITE:
        onWrite(ev, avaliable);
		break;
	default:
		break;
	}
}
void IocpEvent::onRead(Event *ev, uint32_t avaliable)
{
	THEFOX_TRACE_FUNCTION << " avaliable=" << avaliable;
	
	assert(NULL != ev);

	TcpConnection *conn = ev->conn;
	assert(NULL != conn);
	Buffer *readbuf = conn->readBuffer();
	
	readbuf->hasWritten(avaliable);
	conn->addReadBytes(avaliable);

	if (conn->_messageCallback)
		conn->_messageCallback(conn, readbuf, Timestamp::now());
	
    conn->leaveEvent();

	updateRead(ev);
}

void IocpEvent::onWrite(Event *ev, uint32_t avaliable)
{
	THEFOX_TRACE_FUNCTION << " avaliable=" << avaliable;

	assert(NULL != ev);

	TcpConnection *conn = ev->conn;
	assert(NULL != conn);
	Buffer *writebuf = conn->writeBuffer();

	writebuf->retrieve(avaliable);
	conn->addWriteBytes(avaliable);

	if (conn->_writeCompleteCallback)
		conn->_writeCompleteCallback(conn);

    conn->leaveEvent();

	if (0 == writebuf->readableBytes())
        ev->resetWrite();
	else
		updateWrite(ev);
}
