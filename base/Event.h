#ifndef _THEFOX_BASE_EVENT_H_
#define _THEFOX_BASE_EVENT_H_

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else

#endif

#include <base/Types.h>


namespace thefox
{

class Event
{
public:
    Event()
        : _event(NULL)
    {
        _event = ::CreateEvent(NULL, TRUE, FALSE, NULL);
        if (NULL == _event) {
            // 
        }
    }
    ~Event()
    {
        if (NULL != _event)
            CloseHandle(_event);
    }
    
    void set() { ::SetEvent(_event); }
    void reset() { ::ResetEvent(_event); }
    bool wait()
    {
        ::WaitForSingleObject(_event, INFINITE);
        return true;
    }
private:
    THEFOX_DISALLOW_EVIL_CONSTRUCTORS(Event);
    HANDLE _event;                
};

} // namespace thefox

#endif // _THEFOX_BASE_EVENT_H_