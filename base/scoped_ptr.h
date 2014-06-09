/*
* from boost
*/

#ifndef _THEFOX_BASE_SCOPED_PTR_H_
#define _THEFOX_BASE_SCOPED_PTR_H_

namespace thefox
{


//  scoped_ptr mimics a built-in pointer except that it guarantees deletion
//  of the object pointed to, either on destruction of the scoped_ptr or via
//  an explicit reset(). scoped_ptr is a simple solution for simple needs;
//  use shared_ptr or std::auto_ptr if your needs are more complex.

template<class T> class scoped_ptr // noncopyable
{
private:

    T * ptr;

    scoped_ptr(scoped_ptr const &);
    scoped_ptr & operator=(scoped_ptr const &);

    typedef scoped_ptr<T> this_type;

public:

    typedef T element_type;

    explicit scoped_ptr(T * p = 0): ptr(p) // never throws
    {
    }

    ~scoped_ptr() // never throws
    {
        if (0 != ptr) {
            delete ptr;
        }
    }

    void reset(T * p = 0) // never throws
    {
        this_type(p).swap(*this);
    }

    T & operator*() const // never throws
    {
        return *ptr;
    }

    T * operator->() const // never throws
    {
        return ptr;
    }

    T * get() const // never throws
    {
        return ptr;
    }

    bool operator! () const // never throws
    {
        return ptr == 0;
    }

    void swap(scoped_ptr & b) // never throws
    {
        T * tmp = b.ptr;
        b.ptr = ptr;
        ptr = tmp;
    }
};

template<class T> inline void swap(scoped_ptr<T> & a, scoped_ptr<T> & b) // never throws
{
    a.swap(b);
}

// get_pointer(p) is a generic way to say p.get()

template<class T> inline T * get_pointer(scoped_ptr<T> const & p)
{
    return p.get();
}

} // namespace thefox

#endif // _THEFOX_BASE_SCOPEDPTR_H_
