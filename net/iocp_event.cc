#include <net/iocp_event.h>
#include <log/logging.h>
#include <net/tcp_connection.h>
#include <net/event.h>

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

void IocpEvent::addEvent()
{
	//if (CreateIoCompletionPort(handle, _hIocp, key, 0);
}

bool IocpEvent::delConnection(TcpConnection *conn)
{
	if (0 == ::CancelIo((HANDLE)conn->fd())) {
		THEFOX_LOG(ERROR) << "CancelIo() failed";
		return false;
	}
	return true;
}

bool IocpEvent::processEvents(int32_t timer)
{
	DWORD err = 0;
	DWORD bytes = 0;
    ULONG_PTR key = NULL;
	Event *ev = NULL;
    EventOverLapped *ovlp = NULL;
    BOOL ret = FALSE;
    ret = ::GetQueuedCompletionStatus(_hIocp, &bytes, &key, &ovlp, (DWORD)timer);
    
	if (0 == bytes && NULL == key && NULL == ovlp) {
		THEFOX_LOG(INFO) << "GetQueuedCompletionStatus() quit";
        return false;
	}

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
		ovlp->error = err;
    }

	if (NULL == ovlp) {
		THEFOX_LOG(WARN) << "GetQueuedCompletionStatus() returned no operation";
		return true;
	}

	ev = ovlp->ev;

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

	THEFOX_LOG(ERROR) << "iocp event handler: " << ev->handler;

	ev->handler(ev);

	return true;
}

bool IocpEvent::postRead(IoEvent *ev)
{
	
}

bool IocpEvent::postWrite(IoEvent *ev)
{
	
}

bool IocpEvent::postZeroByteRead(IoEvent *ev)
{
	WSABUF wsabuf = {0};
	DWORD nBytes = 0;
    DWORD flags = 0;
	int bytesRecv = ::WSARecv(ev->conn->fd(), &wsabuf, 1, &nBytes,
							&flags, &ev->_ovlp, NULL);
	if (SOCKET_ERROR == bytesRecv && WSA_IO_PENDING != WSAGetLastError()) {
		THEFOX_LOG(ERROR) << "postZeroByteRead() failed!";
		return false;
	}
	return true;
}

void IocpEvent::handleRead(IoEvent *ev)
{

}

void IocpEvent::handleWrite(IoEvent *ev)
{
	
}

void IocpEvent::handleClose(IoEvent *ev)
{
	
}

void IocpEvent::handleError(IoEvent *ev)
{
	
}

void IocpEvent::handleZeroByteRead(IoEvent *ev)
{
	postZeroByteRead(ev);
}
