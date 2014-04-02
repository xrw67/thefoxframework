/*
* @filename Atomic.h
* @brief 原子的数字类
* @author macwe@qq.com
*/

#ifndef _THEFOX_BASE_ATOMIC_H_
#define _THEFOX_BASE_ATOMIC_H_

#include <base/Types.h>

namespace thefox
{
    
inline int32_t atomicInc(volatile int32_t *value)
{
#ifdef WIN32
    InterlockedIncrement(value);
#else
    __sync_fetch_and_add(value, 1);
#endif
    return *value;
}

inline int64_t atomicInc(volatile int64_t *value)
{
#ifdef WIN32
    InterlockedIncrement64(value);
#else
    __sync_fetch_and_add(value, 1);
#endif
    return *value;
}

inline int32_t atomicDec(volatile int32_t *value)
{
#ifdef WIN32
    InterlockedDecrement(value);
#else
    __sync_fetch_and_sub(value, 1);
#endif
    return *value;
}

inline int64_t atomicDec(volatile int64_t *value)
{
#ifdef WIN32
    InterlockedDecrement64(value);
#else
    __sync_fetch_and_sub(value, 1);
#endif
    return *value;
}


class AtomicInteger
{
public:
    AtomicInteger()
        : _value(0)
    {}
    
    T inc()
    { return atomicInc(&_value); }
    
    T dec()
    { return atomicDec(*_value); }
    
private:
    T _value;
};

typedef AtomicInteger<int32_t> AtomicInt32;
typedef AtomicInteger<int64_t> AtomicInt64;

} // namespace thefox

#endif  // _THEFOX_BASE_ATOMIC_H_
