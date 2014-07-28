/**
* @filename condition.h
* @brief 对条件变量的封装，只适用于linux
* @author macwe1024 at gmail dot com
*/

#ifndef _THEFOX_BASE_CONDITION_H_
#define _THEFOX_BASE_CONDITION_H_

#ifdef WIN32
#error "class condition use for linux only"
#else
#include <pthread.h>
#include <base/common.h>
#include <base/mutex.h>
#include <base/time_util.h>

namespace thefox {

class Condition
{
public:
    static const uint32_t kInfinite = 0xffffffff;

    Condition(Mutex &mutex)
        : _mutex(mutex)
    {
		pthread_cond_init(&_cond, NULL);
    }
    ~Condition()
    {
        pthread_cond_destroy(&_cond);
    }
    
    bool wait(uint32_t millisecond = kInfinite)
    {
        int32_t ret = 0;
        
        if (kInfinite == millisecond) {
            ret = pthread_cond_wait(&_cond, _mutex.getMutex());
        } else {
            struct timespec ts = {0, 0};
            TimeUtil::getAbsTimespec(&ts, millisecond);
            ret = pthread_cond_timedwait(&_cond, _mutex.getMutex(), &ts);
        }
        
        return 0 == ret;
    }
    
    bool notify()
    {
        return 0 == pthread_cond_signal(&_cond);
    }
    
    bool notifyAll()
    {
        return 0 == pthread_cond_broadcast(&_cond);
    }
    
private:
    THEFOX_DISALLOW_EVIL_CONSTRUCTORS(Condition);
    
    Mutex& _mutex;
    pthread_cond_t _cond;
};

} // namespace thefox

#endif // #ifdef WIN32 #else

#endif // _THEFOX_BASE_CONDITION_H_
