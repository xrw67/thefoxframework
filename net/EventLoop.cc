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
	: _started(false)
{
    init();
}

EventLoop::~EventLoop()
{
    stop();
    CloseHandle(_hIocp);
}

void EventLoop::init()
{
    _hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	int threads = getCpuNum() * 2;
    for (int i = 0; i < threads; ++i)
		_threads.push_back(ThreadPtr(new Thread(std::bind(&EventLoop::loop, this), "eventloop.loop")));
}

void EventLoop::start()
{
	_started = true;
    for (int i = 0; i < _threads.size(); ++i)
		_threads[i]->start();
}

void EventLoop::join()
{
	for (int i = 0; i < _threads.size(); ++i)
		_threads[i]->join();
}

void EventLoop::stop()
{
	_started = false;
    int threads = getCpuNum() * 2;
    for (int i = 0; i < threads; ++i)
        PostQueuedCompletionStatus(_hIocp, 0, NULL, NULL);

   for (int i = 0; i < _threads.size(); ++i)
		_threads[i]->stop();
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

    while (_started) {
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