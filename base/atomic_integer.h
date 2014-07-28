/**
* @filename atomic_integer.h
* @brief 原子的数字类
* @author macwe1024 at gmail dot com
*/

#ifndef _THEFOX_BASE_ATOMIC_INTEGER_H_
#define _THEFOX_BASE_ATOMIC_INTEGER_H_

#include <base/common.h>

#ifdef WIN32
#include <Winbase.h>
#endif

namespace thefox {
  
#ifdef WIN32
inline LONG atomicInc(volatile LONG *value)
{ return InterlockedIncrement(value); }
#else
inline int32_t atomicInc(volatile int32_t *value)
{
    __sync_fetch_and_add(value, 1);
    return *value;
}
#endif

#ifdef WIN32
inline LONGLONG atomicInc(volatile LONGLONG *value)
{ return InterlockedIncrement64(value); }
#else
inline int64_t atomicInc(volatile int64_t *value)
{
    __sync_fetch_and_add(value, 1);
    return *value;
}
#endif

#ifdef WIN32
inline LONG atomicDec(volatile LONG *value)
{ return InterlockedDecrement(value); }
#else
inline int32_t atomicDec(volatile int32_t *value)
{
    __sync_fetch_and_sub(value, 1);
    return *value;
}
#endif

#ifdef WIN32
inline LONGLONG atomicDec(volatile LONGLONG *value)
{ return InterlockedDecrement64(value); }
#else
inline int64_t atomicDec(volatile int64_t *value)
{
    __sync_fetch_and_sub(value, 1);
    return *value;
}
#endif


template<typename T>
class AtomicInteger
{
public:
    AtomicInteger()
        : _value(0)
    {}
    
    T inc() { return atomicInc(&_value); }
    T dec() { return atomicDec(&_value); }
	T value() const { return _value; }
private:
    T _value;
};

#ifdef WIN32
typedef AtomicInteger<LONG> AtomicInt32;
typedef AtomicInteger<LONGLONG> AtomicInt64;
#else
typedef AtomicInteger<int32_t> AtomicInt32;
typedef AtomicInteger<int64_t> AtomicInt64;
#endif

} // namespace thefox

#endif  // _THEFOX_BASE_ATOMIC_INTEGER_H_
