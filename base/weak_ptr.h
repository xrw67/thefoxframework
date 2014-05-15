#ifndef _THEFOX_WEAKPTR_H_
#define _THEFOX_WEAKPTR_H_

#include <base/shared_ptr.h>

namespace thefox
{
template<typename T>
class weak_ptr
{
public:
    weak_ptr()
    {}
    weak_ptr(const SharedPtr<T> &sp)
    {}
    weak_ptr(const WeakPtr &wp)
    {}
    ~weak_ptr()
    {}
    SharedPtr<T> lock() const
    {}
    
    void reset();
    void swap(weak_ptr<T> &b)
    {}
private:
    PtrCounter _counter;
};   

} // namespace thefox

#endif // _THEFOX_WEAKPTR_H_