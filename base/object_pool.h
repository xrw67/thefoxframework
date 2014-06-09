/*
 * @filename 0bject_pool.h
 * @brief 存放对象元素的内存池,可自动调用构造函数和析构函数
 * @author macwe1024 at gmail dot com
 */

#ifndef _THEFOX_BASE_OBJECT_POOL_H_
#define _THEFOX_BASE_OBJECT_POOL_H_

#include <new>
#include <base/mem_pool.h>

namespace thefox
{

template<typename T>
class ObjectPool
{
public:
    T *get()
    {
        T *obj = _pool.get();
        new(obj) T(); // placement new
        return obj;
    }

    template<typename Arg0>
    T *get(Arg0 &arg0)
    {
        T *obj = _pool.get();
        new(obj) T(arg0); // placement new
        return obj;
    }
    
    template<typename Arg0, typename Arg1>
    T *get(Arg0 &arg0, Arg1 &arg1)
    {
        T *obj = _pool.get();
        new(obj) T(arg0, arg1); // placement new
        return obj;
    }

    template<typename Arg0, typename Arg1, typename Arg2>
    T *get(Arg0 &arg0, Arg1 &arg1, Arg2 &arg2)
    {
        T *obj = _pool.get();
        new(obj) T(arg0, arg1, arg2); // placement new
        return obj;
    }

    template<typename Arg0, typename Arg1, typename Arg2, typename Arg3>
    T *get(Arg0 &arg0, Arg1 &arg1, Arg2 &arg2, Arg3 &arg3)
    {
        T *obj = _pool.get();
        new(obj) T(arg0, arg1, arg2, arg3); // placement new
        return obj;
    }

    template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    T *get(Arg0 &arg0, Arg1 &arg1, Arg2 &arg2, Arg3 &arg3, Arg4 &arg4)
    {
        T *obj = _pool.get();
        new(obj) T(arg0, arg1, arg2, arg3, arg4); // placement new
        return obj;
    }
    
    /*
     * @brief recover object
     */
    void put(T *obj)
    {
        obj->~T();
        _pool.put(obj);
    }

private:
    MemPool<T> _pool;
};
    
} // namespace thefox

#endif // _THEFOX_BASE_OBJECT_MEM_POOL_H_
