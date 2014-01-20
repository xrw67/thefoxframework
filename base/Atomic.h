/*
* @filename Atomic.h
* @brief Ô­×Ó²Ù×÷
* @author macwe@qq.com
*/

#ifndef _THEFOX_ATOMIC_H
#define _THEFOX_ATOMIC_H

#ifdef WIN32

#else
#include <stdint.h>
#endif

namespace muduo
{

inline uint32_t atomicInc(volatile uint32_t *value) 
{
	
#ifdef WIN32
	InterlockedIncrement(value);
#else
	__sync_fetch_and_add(value, 1);
#endif
	return *value;
}

inline uint64_t atomicInc(volatile uint64_t *value) 
{
#ifdef WIN32
	InterlockedIncrement64(value);
#else
	__sync_fetch_and_add(value, 1);
#endif
	return *value;
}

inline uint32_t atomicDec(volatile uint32_t *value)
{
#ifdef WIN32
	InterlockedDecrement(value);
#else
	__sync_fetch_and_sub(value, 1);
#endif
	return *value;
}

inline uint64_t atomicDec(volatile uint64_t *value)
{
#ifdef WIN32
	InterlockedDecrement64(value);
#else
	__sync_fetch_and_sub(value, 1);
#endif
	return *value;
}

} // namespace thefox

#endif  // _THEFOX_ATOMIC_H
