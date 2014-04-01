/*
* @filename Sharedable.h
* @brief 作为基类使用，作为SharedPtr管理的类的父类
* @author macwe@qq.com
*/

#ifndef _THEFOX_BASE_SHAREABLE_H_
#define _THEFOX_BASE_SHAREABLE_H_

#include <base/noncopyable.h>
#include <base/MutexLock.h>

namespace thefox
{

class Shareable : noncopyable
{
public:
	Shareable()
		: _refCount(0)
	{}

	void addRef()
    {
        MutexLockGuard lock(_refMutex);
        ++_refCount;
    }
    
	size_t release()
    {
        MutexLockGuard lock(_refMutex);
        if (_refCount > 0)
            --_refCount;
        return _refCount;
    }
    
	size_t useCount()
    {
        MutexLockGuard lock(_refMutex);
        return _refCount;
    }
private:
	size_t _refCount;
    MutexLock _refMutex;
};

} // namespace thefox

#endif // _THEFOX_BASE_SHAREABELE_H_
