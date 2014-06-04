#ifndef _THEFOX_BASE_CONDITION_H_
#define _THEFOX_BASE_CONDITION_H_

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <pthread.h>
#endif

#include <base/common.h>
#include <base/mutex.h>
#include <base/time_util.h>

namespace thefox
{

class Condition
{
public:
#ifdef _WIN32
    static const uint32_t kInfinite = INFINITE;
#else
    static const unit32_t kInfinite = UINT_MAX
#endif

    Condition(Mutex &mutex)
        : _mutex(mutex)
#ifdef WIN32
        , _waiters(0)
#endif
    {
#ifdef WIN32
       _sem = ::CreateSemaphore(NULL, 0, LONG_MAX, NULL);
#else
        pthread_cond_init(&_cond, NULL);
#endif
    }
    ~Condition()
    {
#ifdef WIN32
        if (NULL != _sem) {
            if (0 != ::CloseHandle(_sem))
                _sem = NULL;
        }
#else
        pthread_cond_destroy(&_cond);
#endif 
    }
    
    bool wait(uint32_t millisecond = kInfinite)
    {
#ifdef WIN32
        if (NULL == _sem)
            return false;
        MutexGuard lock(_mutex);
        ++_waiters;
        DWORD ret = ::WaitForSingleObject(_sem, millisecond);        
        --_waiters;
        
         if (WAIT_OBJECT_0 == ret || WAIT_ABANDONED == ret)
            return true;
        else
            return false;
#else
        int32_t ret = 0;
        
        if (kInfinite == millisecond) {
            ret = pthread_cond_wait(&_cond, _mutex.getMutex());
        } else {
            struct timespec ts = {0, 0};
            TimeUtil::getAbsTimespec(&ts, millisecond);
            ret = pthread_cond_timedwait(&_cond, _mutex.getMutex(), &ts);
        }
        
        return 0 == ret;
#endif
    }
    
    bool notify()
    {
#ifdef WIN32
        BOOL ret = FALSE;
        if (_waiters > 0 && NULL != _sem)
            ret = ::ReleaseSemaphore(_sem, 1, NULL);
            
        return TRUE == ret;    
#else
        return 0 == pthread_cond_signal(&_cond);
#endif
    }
    
    bool notifyAll()
    {
#ifdef WIN32
        BOOL ret = FALSE;
        if (_waiters > 0 && NULL != _sem)
            ret = ::ReleaseSemaphore(_sem, _waiters, NULL);
            
        return TRUE == ret;    
#else
        return 0 == pthread_cond_broadcast(&_cond);
#endif
    }
    
private:
    THEFOX_DISALLOW_EVIL_CONSTRUCTORS(Condition);
    
    Mutex& _mutex;
    
#ifdef WIN32
    HANDLE _sem;
    LONG _waiters; // 等待的线程数量
#else
    pthread_cond_t _cond;
#endif
};

} // namespace thefox

#endif // _THEFOX_BASE_CONDITION_H_
