#include <net/Iocp.h>
#include <net/Event.h>
#include <net/TcpConnection.h>

using namespace thefox;
using namespace thefox::net;

Iocp::Iocp(void)
{
	_hIocp = CreateIoCompletionPort(
		INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (_hIocp == NULL) {
		// Error
	}
}

Iocp::~Iocp(void)
{
}

bool Iocp::associateSocket(SOCKET s, ULONG_PTR CompletionKey)
{
	if (CreateIoCompletionPort(
		    (HANDLE)s, _hIocp, CompletionKey, 0) == NULL) {
	    // Error
        return false;
	}
    return true;
}

Event *Iocp::poll(int timeout)
{
	TcpConnection *conn = NULL;
	DWORD bytesTransfered = 0;
	LPOVERLAPPED *ol = NULL;
	BOOL ret = FALSE;
	
	ret = GetQueuedCompletionStatus(
		_hIocp, &bytesTransfered, (LPDWORD)&conn, ol, timeout);
	
    // process error
    if (!ret) {
        DWORD errCode = GetLastError();
        if (WAIT_TIMEOUT != errCode) {
            Event *evt = conn->getEvent();
            evt->setEvent(Event::EVENT_CLOSE);
		    return evt;
        }
        if (ol) {
            Event *evt = NULL;
		    if ((evt = CONTAINING_RECORD(ol, Event, _overlapped)) != NULL)
			    delete evt;
        }
        return NULL;
    }

    if (ret && conn && ol) {
        Event *evt = NULL;
		if ((evt = CONTAINING_RECORD(ol, Event, _overlapped)) != NULL)
			return evt;
    }

    if (NULL == conn && NULL == ol)
        return NULL;

    return NULL;
}