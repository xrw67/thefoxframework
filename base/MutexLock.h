/*
* 互斥量操作类
* @author macwe@qq.com
*/

#ifndef _THEFOX_MUTEXLOCK_H_
#define _THEFOX_MUTEXLOCK_H_

namespace thefox
{
class MutexLock
{
	MutexLock()
	: _threadId(0)
	{
		::InitializeCriticalSection(&_cs);
	}
	~MutexLock()
	{
		ASSERT(0 == _threadId);
		::DeleteCriticalSection(&_cs);
	}
	
	void Lock()
	{
		::EnterCriticalSection(&_cs);
		_threadId = ::GetCurrentThreadId();
	}
	
	void Unlock()
	{
		_threadId = 0;
		::LeverCriticalSection(&_cs);
	}
	
private:
	MutexLock(const MutexLock &);
	const MutexLock &operator=(const MutexLock &);
	CRITICAL_SECTION _cs;
	DWORD _threadId;
};

class MutexLockGuard
{
public:
	MutexLockGuard(const MutexLock &mutex)
		: _mutex(mutex)
	{
		_mutex.Lock();
	}
	~MutexLockGuard()
	{
		_mutex.Unlock();
	}
private:
	MutexLockGuard(const MutexLockGuard &);
	const MutexLockGuard &operator=(const MutexLockGuard &);
	MutexLock &_mutex;
};

#define MutexLockGuard(x) error "Missing guard object name"

}

#endif // _THEFOX_MUTEXLOCK_H_