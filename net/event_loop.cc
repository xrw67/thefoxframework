#include <net/event_loop.h>
#ifdef WIN32
#include <net/iocp_event.h>
#else
#include <net/epoll_event.h>
#endif

#include <base/thread.h>

using namespace thefox;

EventLoop::EventLoop()
	: _started(false)
#ifdef WIN32
	, _poller(new IocpEvent())
#else
	, _poller(new EpollEvent())
#endif
{
    init();
}

EventLoop::~EventLoop()
{
    stop();
    CloseHandle(_hIocp);

	for (size_t i = 0; i < _threads.size(); ++i)
		delete _threads[i];
	_threads.clear();
}

void EventLoop::init()
{
    _poller->init();
	int threads = getCpuNumber() * 2;
    for (int i = 0; i < threads; ++i)
		_threads.push_back(new Thread(std::bind(&EventLoop::loop, this), "eventloop.loop"));
}

void EventLoop::start()
{
	_started = true;
    for (size_t i = 0; i < _threads.size(); ++i)
		_threads[i]->start();
}

void EventLoop::join()
{
	for (size_t i = 0; i < _threads.size(); ++i)
		_threads[i]->join();
}

void EventLoop::stop()
{
	_started = false;
    int threads = getCpuNumber() * 2;
    for (int i = 0; i < threads; ++i)
        PostQueuedCompletionStatus(_hIocp, 0, NULL, NULL);

   for (size_t i = 0; i < _threads.size(); ++i)
		_threads[i]->stop();
}

bool EventLoop::delConnection(TcpConnection *conn)
{
	return _poller->delConnection(conn);
}

void EventLoop::loop()
{
	for (;;) {
		// 计算下一次超时时间
		uint32_t time = 1000;
		
		//
		_poller->processEvents(time);
	}
}

int EventLoop::getCpuNumber()
{
#ifdef WIN32
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return si.dwNumberOfProcessors;
#else
	return 2;
#endif
}