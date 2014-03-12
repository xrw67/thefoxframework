#include <net/Iocp.h>
#include <net/TcpConnection.h>

using namespace thefox;
using namespace thefox::net;

Iocp::Iocp(void)
    : _quit(false)
{
	_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
}

Iocp::~Iocp(void)
{
    _quit = true;
}

bool Iocp::registerSocket(SOCKET s, TcpConnection *conn)
{
	if (CreateIoCompletionPort((HANDLE)s, _hIocp, (ULONG_PTR)conn, 0) == NULL)
        return false;
    return true;
}

bool Iocp::postCompletion(IoContext *buf, TcpConnection *conn, DWORD bytesTransferred)
{
    BOOL result =  PostQueuedCompletionStatus(_hIocp, bytesTransferred, (ULONG_PTR)conn, &buf->_overlapped);
    if (!result && GetLastError() != ERROR_IO_PENDING)
        return false;
    return true;
}

void Iocp::loop()
{
	DWORD bytesTransfered = 0;
    TcpConnection *conn = NULL;
	OVERLAPPED *overlapped = NULL;
	BOOL ret = FALSE;
	
    while (!_quit) {
	    ret = GetQueuedCompletionStatus(_hIocp,
                                        &bytesTransfered,
                                        (LPDWORD)&conn, 
                                        &overlapped,
                                        INFINITE);
        if (!ret) {
            DWORD errCode = GetLastError();
            if (conn && WAIT_TIMEOUT != errCode)
                conn->handleClose();
            if (overlapped) {
                IoContext *io = NULL;
		        if ((io = CONTAINING_RECORD(overlapped, IoContext, _overlapped)) != NULL)
                    IoContextPool::Instance().put(io);
            }
            continue;
        }

        if (ret && conn && overlapped) {
            IoContext *io = NULL;
		    if ((io = CONTAINING_RECORD(overlapped, IoContext, _overlapped)) != NULL)
                conn->handleEvent(io);
        }

        if (NULL == conn && NULL == overlapped)
            break;
    }
}