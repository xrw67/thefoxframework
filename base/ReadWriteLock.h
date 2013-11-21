/*
* ¶ÁÐ´Ëø
*/
#ifndef _THEFOX_READWRITELOCK_H_
#endif _THEFOX_READWRITELOCK_H_

#include <base/noncopyable.h>

namespace thefox
{

class ReadWriteLock : noncopyable
{
public:
	ReadWriteLock()
		: _reads(0)
	{ 
		::InitializeCriticalSection(&_enterLock); 
	}
	
	~ReadWriteLock()
	{ 
		::DeleteCriticalSection(&_enterLock); 
	}
	
	void readLock()
	{
		::EnterCriticalSection(&_enterLock);
		::InterlockedIncrement(&_reads);
		::LeaveCriticalSection(&_enterLock);
	}
	
	void readUnlock()
	{
		::InterlockedDecrement(&_reads);
	}
	
	void writeLock()
	{
		::EnterCriticalSection(&_enterLock);
		while (_reads > 0)
		{
			Sleep(0);
		}
	}
	
	void writeUnlock()
	{
		::LeaveCriticalSection(&_enterLock);
	}
	
private:
	CRITICAL_SECTION _enterLock;
	LONG _reads;
};

class ReadLockGuard : noncpoyable
{
public:
	ReadLockGuard(ReadWriteLock &lock)
		: _lock(lock)
	{
		_lock.readLock();
	}
	~ReadLockGuard()
	{
		_lock.readUnlock();
	}
private:
	ReadWriteLock &_lock;
};

class WriteLockGuard : noncpoyable
{
public:
	WriteLockGuard(ReadWriteLock &lock)
		: _lock(lock)
	{
		_lock.writeLock();
	}
	~WriteLockGuard()
	{
		_lock.writeUnlock();
	}
private:
	ReadWriteLock &_lock;
};

#define ReadLockGuard(x) error "Missing guard object name"
#define WriteLockGuard(x) error "Missing guard object name"
}

#endif // _THEFOX_READWRITELOCK_H_