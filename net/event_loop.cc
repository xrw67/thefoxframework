#include <net/event_loop.h>
#include <log/logging.h>

#ifdef WIN32
    #include <net/iocp_event.h>
#else
    #include <net/epoll_event.h>
#endif

namespace thefox {
namespace net {

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

} // namespace net
} // namespace thefox

using namespace thefox;
using namespace thefox::net;

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
        Thread *thread = new Thread(std::bind(&EventLoop::loop, this));
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

bool EventLoop::updateRead(Event *ev)
{
	return _poller->updateRead(ev);
}

bool EventLoop::updateWrite(Event *ev)
{
	return _poller->updateWrite(ev);
}

bool EventLoop::addEvent(Event *ev)
{
	return _poller->addEvent(ev);
}

bool EventLoop::delEvent(Event *ev)
{
	return _poller->delEvent(ev);
}

void EventLoop::loop()
{
	for (;;) {
		// 计算下一次超时时间
		uint32_t time = 10000000;
		
		//
		_poller->processEvents(time);
	}
}
