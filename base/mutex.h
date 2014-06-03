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
typedef CRITICAL_SECTION pthread_mutex_t;
#else
#include <pthread.h>
#endif

#include <base/common.h>

namespace thefox 
{

class Mutex
{
public:
    Mutex()
        : _threadId(0)
    {
#ifdef WIN32
        InitializeCriticalSection(&_mutex);
#else
        pthread_mutex_init(&_mutex, NULL);
#endif
    }
    ~Mutex()
    {
#ifdef WIN32
        DeleteCriticalSection(&_mutex);
#else
		pthread_mutex_destroy(&_mutex);
#endif
    }
    
    void lock()
    {
#ifdef WIN32
        EnterCriticalSection(&_mutex);
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
        LeaveCriticalSection(&_mutex);
#else
        pthread_mutex_unlock(&_mutex);
#endif
    }
    
	pthread_mutex_t *getMutex()
	{
		return &_mutex;
	}
	
private:
    THEFOX_DISALLOW_EVIL_CONSTRUCTORS(Mutex);
	
    uint32_t _threadId;
    pthread_mutex_t _mutex;
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
