#include <net/TimerQueue.h>
#include <net/EventLoop.h>

// http://msdn.microsoft.com/en-us/library/windows/desktop/ms687008(v=vs.85).aspx

namespace thefox
{

HANDLE createTimerHandle()
{
    HANDLE timer = CreateWaitableTimer(NULL, TRUE, NULL);
    if (NULL == timer) {
        // LOG_ERROR<< create timer failed;
    }
    return timer;
}

DWORD WINAPI timerThreadProc(LPVOID param)
{
    TimerQueue *t = reinterpret_cast<TimerQueue *>(param);
    t->timerProc();
    return 0;
}

} // namespace thefox

using namespace thefox;

TimerQueue::TimerQueue(EventLoop *loop)
    : _eventloop(loop)
    , _hTimer(createTimerHandle())
{
}

TimerQueue::~TimerQueue()
{
    CloseHandle(_hTimer);
}

void TimerQueue::addTimer(Timestamp when, IoEvent *ev)
{
    {
        MutexLockGuard lock(_mutex);
        _timers.insert(Entry(when, ev));
    }

	SetWaitableTimer(_hTimer, 
        &_timers.begin()->first.toLargeInt(), 0, NULL, NULL, 0);
}

void TimerQueue::timerProc()
{
    while (true) {
        if (WAIT_OBJECT_0 == WaitForSingleObject(_hTimer, INFINITE)) {
            MutexLockGuard lock(_mutex);
            Timestamp now(Timestamp::now());
            while (!_timers.empty()) {
                if (_timers.begin()->first <= now) {
                    _eventloop->postEvent(_timers.begin()->second);
                    _timers.erase(_timers.begin());
                }
            }
        } else {
            // LOG_ERROR
        }
    }
}