#ifndef _THEFOX_NET_TIMERQUEUE_H_
#define _THEFOX_NET_TIMERQUEUE_H_

#include <set>
#include <Windows.h>
#include <base/Types.h>
#include <base/MutexLock.h>
#include <base/Timestamp.h>

namespace thefox
{

class IoEvent;
class EventLoop;

class TimerQueue
{
public:
    TimerQueue(EventLoop *loop);
    ~TimerQueue();
    void addTimer(Timestamp when, IoEvent *ev);
    void timerProc();
    
private:
    typedef std::pair<Timestamp, IoEvent *> Entry;
    typedef std::set<Entry> TimerList;

    EventLoop *_eventloop;
    HANDLE _hTimer; 
    TimerList _timers;
    MutexLock _mutex;
};

} // namespace thefox

#endif // _THEFOX_NET_TIMERQUEUE_H_