#include <net/EventLoop.h>
#include <net/IoEvent.h>

namespace thefox
{

DWORD WINAPI eventloopThreadProc(LPVOID param)
{
	EventLoop *loop =  reinterpret_cast<EventLoop *>(param);
	loop->loop();
	return 0;
}

} // namespace thefox

using namespace thefox;

EventLoop::EventLoop()
{
	init();
}

EventLoop::~EventLoop()
{
	quit();
	CloseHandle(_hQuitEvent);
	CloseHandle(_hIocp);
}

void EventLoop::init()
{
	_hQuitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
}

void EventLoop::start()
{
	ResetEvent(_hQuitEvent);

	HANDLE handle;
	int threads = getCpuNum() * 2;
	for (int i = 0; i < threads; ++i) {
		handle = CreateThread(NULL, 0, eventloopThreadProc, this, 0, NULL);
		CloseHandle(handle);
	}
}

void EventLoop::join()
{
	WaitForSingleObject(_hQuitEvent, INFINITE);
}

void EventLoop::quit()
{
	int threads = getCpuNum() * 2;
	for (int i = 0; i < threads; ++i)
		PostQueuedCompletionStatus(_hIocp, 0, NULL, NULL);

	SetEvent(_hQuitEvent);
}

void EventLoop::registerHandle(HANDLE handle)
{
	CreateIoCompletionPort(handle, _hIocp, 0, 0);
}

void EventLoop::postEvent(IoEvent *e)
{
	if (NULL != e) {
		PostQueuedCompletionStatus(_hIocp, 0, NULL, &e->_overlapped);
	} else {
		//LOG_ERROR<<post a NULL event;
	}
}

void EventLoop::loop()
{
	DWORD bytesTransfered = 0;
    ULONG_PTR CompletionKey = NULL;
	OVERLAPPED *overlapped = NULL;
	BOOL ret = FALSE;

	while (WAIT_OBJECT_0 != WaitForSingleObject(_hQuitEvent, 0)) {
		ret = GetQueuedCompletionStatus(_hIocp, 
										&bytesTransfered, 
										&CompletionKey, 
										&overlapped, 
										1000);
		if (!ret) {
            if (overlapped && WAIT_TIMEOUT != GetLastError()) {
                IoEvent *e = NULL;
		        if ((e = CONTAINING_RECORD(overlapped, IoEvent, _overlapped)) != NULL)
					e->handleError();
            }
            continue;
        }

		if (ret && overlapped) {
			IoEvent *e = NULL;
			if ((e = CONTAINING_RECORD(overlapped, IoEvent, _overlapped)) != NULL) {
				e->setBytesTransfered(bytesTransfered);
				e->handleEvent();
			}
		}

		if (0 == bytesTransfered && NULL == CompletionKey && NULL == overlapped)
            break;
	}
}

int EventLoop::getCpuNum()
{
	SYSTEM_INFO si;
    GetSystemInfo(&si);
	return si.dwNumberOfProcessors;
}