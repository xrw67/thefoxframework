/*
* @filename MutexLock.h
* @brief 互斥量操作类, 支持Windows和Linux
* @author macwe@qq.com
*/

#ifndef _THEFOX_MUTEXLOCK_H_
#define _THEFOX_MUTEXLOCK_H_

#ifdef WIN32
#include <Windows.h>
#else
#include <pthread.h>
#endif

#include <base/noncopyable.h>

namespace thefox 
{

class MutexLock : noncpoyable
{
public:
	MutexLock()
		: _threadId(0)
	{
#ifdef WIN32
		InitializeCriticalSection(&_cs);
#else
		_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif
	}
	~MutexLock()
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
	uint32_t _threadId;
#ifdef WIN32
	CRITICAL_SECTION _cs;
#else
	pthread_mutex_t _mutex;
#endif

};

class MutexLockGuard : noncpoyable
{
public:
	MutexLockGuard(MutexLock &mutex)
		: _mutex(mutex)
	{
		_mutex.lock();
	}
	~MutexLockGuard()
	{
		_mutex.unlock();
	}
private:
	MutexLock &_mutex;
};

#define MutexLockGuard(x) error "Missing guard object name"

} // namespace thefox

#endif // _THEFOX_MUTEXLOCK_H_
