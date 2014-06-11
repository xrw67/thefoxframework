#ifndef _THEFOX_NET_SEMAPHORE_H_
#define _THEFOX_NET_SEMAPHORE_H_

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include<semaphore.h>
#endif

#include <base/common.h>
#include <base/time_util.h>

namespace thefox
{

class Semaphore
{
public:
#ifdef _WIN32
    static const uint32_t kInfinite = INFINITE;
    typedef HANDLE sem_t;
#else
    static const uint32_t kInfinite = UINT_MAX;
#endif

    Semaphore(int32_t num = 0)
    {
#ifdef _WIN32
        _sem = ::CreateSemaphore(NULL, num, LONG_MAX, NULL);
#else
        sem_init(&_sem, 0, num);
#endif    
    }
    ~Semaphore()
    {
#ifdef _WIN32
        if (NULL != _sem) {
            if (0 != ::CloseHandle(_sem)) {
                _sem = NULL;
            }
        }
#else
        sem_destroy(&_sem);
#endif    
    }
    
	// P
    bool wait(uint32_t millisecond = kInfinite)
    {
#ifdef _WIN32
        if (NULL == _sem)
            return false;

        DWORD ret = ::WaitForSingleObject(_sem, millisecond);
        if (WAIT_OBJECT_0 == ret || WAIT_ABANDONED == ret) {
            return true;
        } else {
            return false;
        }
#else
        int32_t ret = 0;

        if (kInfinite == millisecond) {
            ret = sem_wait(&_sem);
        } else {
            struct timespec ts = {0, 0};
            TimeUtil::getAbsTimespec(&ts, millisecond);
            ret = sem_timedwait(&_sem, &ts);
        }

        return -1 != ret;
#endif
    }
    
	// V
    bool signal()
    {
#ifdef _WIN32
        BOOL ret = FALSE;
        
        if (NULL != _sem) {
            ret = ::ReleaseSemaphore(_sem, 1, NULL);
        }
        return TRUE == ret;
#else
        return -1 != sem_post(&_sem);
#endif    
    }
    
private:
    THEFOX_DISALLOW_EVIL_CONSTRUCTORS(Semaphore);
    sem_t _sem;
};

} // namespace thefox
#endif // _THEFOX_NET_SEMAPHORE_H_