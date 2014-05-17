/*
 * @filename 0bject_pool.h
 * @brief 存放对象元素的内存池,可自动调用构造函数和析构函数
 * @author macwe@qq.com
 */

#ifndef _THEFOX_BASE_OBJECT_POOL_H_
#define _THEFOX_BASE_OBJECT_POOL_H_

#include <new>
#include <base/mem_pool.h>

namespace thefox
{

template<typename T, int kBlockSize = 32>
class ObjectPool : MemPool<T, kBlockSize>
{
public:
    /// @brief 获取分配的内存
    T *get()
    {
        T *obj = MemPool::get();
        new(obj) T(); // placement new
        return obj;
    }
    template<typename Arg0>
    T *get(const Arg0 &arg0)
    {
        T *obj = MemPool::get();
        new(obj) T(arg0); // placement new
        return obj;
    }
    
    template<typename Arg0, typename Arg1>
    T *get(const Arg0 &arg0, const Arg1 &arg1)
    {
        T *obj = MemPool::get();
        new(obj) T(arg0, arg1); // placement new
        return obj;
    }

    template<typename Arg0, typename Arg1, typename Arg2>
    T *get(const Arg0 &arg0, const Arg1 &arg1, const Arg2 &arg2)
    {
        T *obj = MemPool::get();
        new(obj) T(arg0, arg1, arg2); // placement new
        return obj;
    }
    template<typename Arg0, typename Arg1, typename Arg2, typename Arg3>
    T *get(const Arg0 &arg0, const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3)
    {
        T *obj = MemPool::get();
        new(obj) T(arg0, arg1, arg2, arg3); // placement new
        return obj;
    }
    template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    T *get(const Arg0 &arg0, const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4)
    {
        T *obj = MemPool::get();
        new(obj) T(arg0, arg1, arg2, arg3, arg4); // placement new
        return obj;
    }
    
    // @brief 回收对象
    void put(T *obj)
    {
        pointer->~T();
        MemPool::put(obj);
    }
};
    
} // namespace thefox

#endif // _THEFOX_BASE_OBJECT_MEM_POOL_H_
