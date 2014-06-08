#include <net/event_loop.h>
#include <base/thread.h>
#include <log/logging.h>

#ifdef WIN32
    #include <net/iocp_event.h>
#else
    #include <net/epoll_event.h>
#endif

namespace thefox
{

inline int getThreadNum()
{
#ifdef WIN32
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return static_cast<int>(si.dwNumberOfProcessors) * 2;
#else
    return 1;
#endif
}

} // namespace thefox

using namespace thefox;

EventLoop::EventLoop()
    : _started(false)
    , _poller(new Poller())
    , _threadNum(getThreadNum())
{
    _poller->init();
}

EventLoop::~EventLoop()
{
    stop();

    for (size_t i = 0; i < _threads.size(); ++i) {
        delete _threads[i];
    }
	_threads.clear();
}

void EventLoop::start()
{
	THEFOX_TRACE_FUNCTION;

	_started = true;

    for (int i = 0; i < _threadNum; ++i) {
        Thread *thread = new Thread(std::bind(&EventLoop::loop, this), "EventLoop::loop");
        _threads.push_back(thread);
        thread->start();
    }
}

void EventLoop::join()
{
	THEFOX_TRACE_FUNCTION;

    for (size_t i = 0; i < _threads.size(); ++i) {
        _threads[i]->join();
    }
}

void EventLoop::stop()
{
	THEFOX_TRACE_FUNCTION;

	_started = false;

    for (size_t i = 0; i < _threads.size(); ++i) {
        _threads[i]->stop();
    }
}

bool EventLoop::updateRead(TcpConnection *conn)
{
	return _poller->updateRead(conn);
}

bool EventLoop::updateWrite(TcpConnection *conn)
{
	return _poller->updateWrite(conn);
}

bool EventLoop::registerConnection(TcpConnection *conn)
{
	return _poller->registerConnection(conn);
}

bool EventLoop::unregisterConnection(TcpConnection *conn)
{
	return _poller->unregisterConnection(conn);
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
