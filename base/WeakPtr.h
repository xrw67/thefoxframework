#ifndef _THEFOX_WEAKPTR_H_
#define _THEFOX_WEAKPTR_H_

#include <base/SharedPtr.h>

namespace thefox
{
template<typename T>
class WeakPtr
{
public:
    WeakPtr()
    {}
    WeakPtr(const SharedPtr<T> &sp)
    {}
    WeakPtr(const WeakPtr &wp)
    {}
    ~WeakPtr()
    {}
    SharedPtr<T> lock() const
    {}
    
    void reset();
    void swap(WeakPtr<T> &b)
    {}
private:
    PtrCounter _counter;
}    

} // namespace thefox

#endif // _THEFOX_WEAKPTR_H_