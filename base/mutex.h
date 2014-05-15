/*
* @filename Mutex.h
* @brief 互斥量操作类, 支持Windows和Linux
* @author macwe@qq.com
*/

#ifndef _THEFOX_BASE_MUTEX_H_
#define _THEFOX_BASE_MUTEX_H_

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <pthread.h>
#endif

#include <base/Types.h>

namespace thefox 
{

class Mutex
{
public:
    Mutex()
        : _threadId(0)
    {
#ifdef WIN32
        InitializeCriticalSection(&_cs);
#else
        _mutex = PTHREAD_MUTEX_INITIALIZER;
#endif
    }
    ~Mutex()
    {
#ifdef WIN32
        DeleteCriticalSection(&_cs);
#endif
    }
    
    void lock()
    {
#ifdef WIN32
        EnterCriticalSection(&_cs);
        _threadId = static_cast<uint32_t>(GetCurrentThreadId());
#else
        pthread_mutex_lock(&_mutex);
        _threadId = static_cast<uint32_t>(pthread_self());
#endif
    }
    
    void unlock()
    {
        _threadId = 0;
#ifdef WIN32
        LeaveCriticalSection(&_cs);
#else
        pthread_mutex_unlock(&_mutex);
#endif
    }
    
private:
    THEFOX_DISALLOW_EVIL_CONSTRUCTORS(Mutex);
    uint32_t _threadId;
#ifdef WIN32
    CRITICAL_SECTION _cs;
#else
    pthread_mutex_t _mutex;
#endif
};

class MutexGuard
{
public:
    MutexGuard(Mutex &mutex)
        : _mutex(mutex)
    {
        _mutex.lock();
    }
    ~MutexGuard()
    {
        _mutex.unlock();
    }
private:
    THEFOX_DISALLOW_EVIL_CONSTRUCTORS(MutexGuard);
    Mutex &_mutex;
};

#define MutexGuard(x) error "Missing guard object name"

} // namespace thefox

#endif // _THEFOX_BASE_MUTEXLOCK_H_
