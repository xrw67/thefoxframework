#ifndef _THEFOX_BASE_EVENT_H_
#define _THEFOX_BASE_EVENT_H_

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <base/common.h>

namespace thefox
{

class Event
{
public:
    static const uint32_t kInfinite = INFINITE;
    Event()
        : _event(NULL)
    {
        _event = ::CreateEvent(NULL, TRUE, FALSE, NULL);
        assert (NULL != _event);
    }
    ~Event()
    {
        if (NULL != _event) {
            CloseHandle(_event);
        }
    }
    
    void set() { ::SetEvent(_event); }
    void reset() { ::ResetEvent(_event); }
    bool wait(DWORD dwMilliseconds = kInfinite)
    {
        ::WaitForSingleObject(_event, dwMilliseconds);
        return true;
    }
	operator HANDLE() { return _event; }
private:
    THEFOX_DISALLOW_EVIL_CONSTRUCTORS(Event);
    HANDLE _event;                
};

} // namespace thefox

#else // #ifdef WIN32
#error "class Event use for Windows only"
#endif

#endif // _THEFOX_BASE_EVENT_H_