/*
* @filename ReadWriteLock.h
* @brief 读写锁
* @author macwe@qq.com
*/

#ifndef _THEFOX_BASE_READWRITELOCK_H_
#define _THEFOX_BASE_READWRITELOCK_H_

#include <base/noncopyable.h>
#include <base/Atomic.h>
#include <base/MutexLock.h>

namespace thefox
{

class ReadWriteLock : noncopyable
{
public:
    ReadWriteLock()
        : _reads(0)
    {}
    
    ~ReadWriteLock()
    {}
    
    void readLock()
    {
        _enterLock.lock();
        atomicInc(&_reads);
        _enterLock.unlock();
    }
    
    void readUnlock()
    {
        atomicDec(&_reads);
    }
    
    void writeLock()
    {
        _enterLock.lock();
        while (_reads > 0)
            Sleep(0);
    }
    
    void writeUnlock()
    {
        _enterLock.unlock();
    }
    
private:
    volatile uint32_t _reads;
    MutexLock _enterLock;
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

#endif // _THEFOX_BASE_READWRITELOCK_H_