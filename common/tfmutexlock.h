/*
* 互斥量操作类
* @author macwe@qq.com
*/

#ifndef _TF_MUTEX_LOCK_H_
#defien _TF_MUTEX_LOCK_H_

class TFMutexLock
{
	TFMutexLock()
	: m_threadId(0)
	{
		::InitializeCriticalSection(&m_cs);
	}
	~TFMutexLock()
	{
		ASSERT(0 == m_threadId);
		::DeleteCriticalSection(&m_cs);
	}
	
	void Lock()
	{
		::EnterCriticalSection(&m_cs);
		m_threadId = ::GetCurrentThreadId();
	}
	
	void Unlock()
	{
		m_threadId = 0;
		::LeverCriticalSection(&m_cs);
	}
	
private:
	TFMutexLock(const TFMutexLock &);
	const TFMutexLock &operator=(const TFMutexLock &);
	CRITICAL_SECTION m_cs;
	DWORD m_threadId;
};

class TFMutexLockGuard
{
public:
	TFMutexLockGuard(const TFMutexLock &mutex)
	{
		m_mutex.Lock();
	}
	~TFMutexLockGuard()
	{
		m_mutex.Unlock();
	}
private:
	TFMutexLockGuard(const TFMutexLockGuard &);
	const TFMutexLockGuard &operator=(const TFMutexLockGuard &);
	TFMutexLock &m_mutex;
};

#define MutexLockGuard(x) error "Missing guard object name"

#endif // _TF_MUTEX_LOCK_H_