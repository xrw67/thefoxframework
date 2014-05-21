#include <net/event_loop.h>
#include <net/iocp_event.h>

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
    _poller.init();
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

bool EventLoop::delConnection(TcpConnection *conn)
{
	_poller.delConnection(conn);
}

void EventLoop::loop()
{
	for (;;) {
		// 计算下一次超时时间
		unit32_t time;
		
		//
		_poller.processEvents(time);
	}
}

int EventLoop::getCpuNum()
{
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return si.dwNumberOfProcessors;
}