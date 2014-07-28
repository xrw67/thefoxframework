/**
* author event_loop.h
* @brief 事件循环
* @author macwe1024 at gmail dot com
*/

#ifndef _THEFOX_NET_EVENTLOOP_H_
#define _THEFOX_NET_EVENTLOOP_H_

#include <vector>
#include <base/common.h>
#include <base/thread.h>

namespace thefox {
namespace net {

class Event;

#ifdef WIN32
    class IocpEvent;
    #define Poller IocpEvent
    typedef void *HANDLE;
#else
    class EpollEvent;
    #define Poller EpollEvent
#endif

class EventLoop
{
public:
    EventLoop();
    ~EventLoop();

    // for user

    /// @brief start event loop
    void start();

    /// @brief blocking and wait thread quit
    void join();

    /// @brief quit thread
    void stop();

    //void setTimer(uint32_t time, const TimerCallback &cb);

    // for internal
    bool updateRead(Event *ev);
    bool updateWrite(Event *ev);
    bool addEvent(Event *ev);
    bool delEvent(Event *ev);

private:
    THEFOX_DISALLOW_EVIL_CONSTRUCTORS(EventLoop);

    void loop();

    Poller *_poller;
    bool _started;
    const int _threadNum; // number of threads
    std::vector<Thread *> _threads;
};

} // namespace net
} // namespace thefox

#endif // _THEFOX_NET_EVENTLOOP_H_