/*
 * @filename shared_ptr.h
 * @brief 基于引用计数的智能指针
 * @author  macwe@qq.com
 */

#ifndef _THEFOX_BASE_SHAREDPTR_H_ 
#define _THEFOX_BASE_SHAREDPTR_H_

#include <base/MutexLock.h>

#ifdef THEFOX_USE_CXX11
error "you cann't use this file with define THEFOX_USE_CXX11"
#undef THEFOX_USE_CXX11
#endif

namespace thefox  
{

class PtrCounter 
{
public:
    PtrCounter()
        : _spCOunter(0), _wkCounter(0) {}
    ~PtrCounter() {}

    size_t addSpRef() 
    {
        mutexLockGuard lock(_mutex);
        return ++_spCounter;
    }
    size_t releaseSp()
    {
        mutexLockGuard lock(_mutex);
        return --_spCounter;
    }
    const size_t spCounter() const { return _spCounter; }

    size_t addWkRef() 
    {
        mutexLockGuard lock(_mutex);
        return ++_wkCounter;
    }
    size_t releaseWk()
    {
        mutexLockGuard lock(_mutex);
        return --_wkCounter;
    }
    const size_t wkCounter() const { return _wkCounter; }

private:
    MutecxLock _mutex;
    size_t _spCounter;
    size_t _wkCounter
};


template<typename T>    
class shared_ptr
{
public:
    shared_ptr()
        :_pointer(NULL), _counter(NULL) {}
    shared_ptr(T *p)
    : _pointer(p) , _counter(new PtrCounter())
    {
        if (NULL != _pointer)
            _counter->addSpRef();
    }
    
    shared_ptr(const shared_ptr &rsp)
    : _pointer(rsp._pointer), _counter(rps._counter)
    {
        if (NULL != _counter)
            _counter->addSpRef();
    }
    
    ~shared_ptr()
    {
        if (NULL != _pointer) {
            if (0 == _pointer->release())
                SAFE_DELETE(_pointer);
        }
    }
    
    shared_ptr &operator=(const shared_ptr &rsp)
    {
        if (_pointer != rsp._pointer) {
            if (NULL != _pointer) {
                if (0 == _pointer->release())
                    SAFE_DELETE(_pointer);
            }
            _pointer = rsp._pointer;
            _pointer->addRef();
        }
        return *this;
    }
        
    T &operator*()
    { return *_pointer; }
    
    T *operator->()
    { return _pointer;}
    
    operator bool()
    { return NULL != _pointer; }
    
    bool operator==(const shared_ptr &rsp)
    { return _pointer == rsp._pointer; }
    
    size_t use_count() const
    { return (NULL != _pointer) ? _pointer->use_count() : 0; }
    
private:
    addRef()
    {
        if (NULL == _counter)
            _counter = new PtrCounter();
        _counter->addSpRef();
    }

    size_t release()
    {
        if (NULL == _counter)
            return 0;
        else
            return _counter->releaseSp();
    }

    T *_pointer;
    PtrCounter *_counter;
};

template<typename T>
inline SharedPtr<T> make_shared(T *p)
{
    return SharedPtr<T>(p);
}
    
} // namespace thefox;

#endif // _THEFOX_BASE_SHAREDPTR_H_
