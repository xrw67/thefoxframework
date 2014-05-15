/*
 * @filename MemPool.h
 * @brief 可存放固定大小元素的内存池
 * @author macwe@qq.com
 */

#ifndef _THEFOX_BASE_MEM_POOL_H_
#define _THEFOX_BASE_MEM_POOL_H_

#include <vector>
#include <base/types.h>
#include <base/mutex.h>

namespace thefox
{

template<typename T, int kBlockSize = 32>
class MemPool
{
public:
    MemPool()
    {
        addChunk();
        _freeHead = _chunks.back();
    }
    ~MemPool()
    {
        while (!_chunks.empty()) {
            T *pointer = _chunks.back();
            _chunks.pop_back();
            free(pointer);
        }
        
        _freeBlocks.clear();
        _freeHead = NULL;
    }
    
    /// @brief 获取分配的内存
    T *get()
    {
        T *ret = NULL;
        
        MutexGuard lock(_mutex);
        if (!_freeBlocks.empty()) {
            ret = _freeBlocks.back();
            _freeBlocks.pop_back();
        } else {
            if (_freeHead == (_chunks.back() + (_chunks.size() * kBlockSize))) {
                addChunk();
                _freeHead = _chunks.back();
            }
            ret = _freeHead;
            ++_freeHead;
        }
        return ret;
    }
    
    void put(T *pointer)
    {
        MutexGuard lock(_mutex);
        _freeBlocks.push_back(pointer);
    }
    
private:
    THEFOX_DISALLOW_EVIL_CONSTRUCTORS(MemPool);
    void addChunk()
    {
        size_t blockSize = (_chunks.size() + 1) * kBlockSize;
        _chunks.push_back(reinterpret_cast<T *>(malloc(sizeof(T) * blockSize)));
    }
    
    std::vector<T *> _chunks;
    T *_freeHead;
    std::vector<T *> _freeBlocks;
    Mutex _mutex;
};
    
} // namespace thefox

#endif // _THEFOX_BASE_MEMPOOL_H_