#ifndef _THEFOX_SCOPED_PTR_H_
#define _THEFOX_SCOPED_PTR_H_

#include <base/noncopyable.h>

namespace thefox
{

template<typename T> class scoped_ptr : noncopyable
{
public:
	typedef T element_type;

	explicit scoped_ptr(T *p = 0)
		: _ptr(p)
	{
	}
	
	~scoped_ptr()
	{
		if (_ptr)
		{
			delete _ptr;
			_ptr = NULL;
		}
	}

	void reset(T *p = 0)
	{
		this_type(p).swap(*this);
	}

	T & operator*() const // never throws
    {
        return *_ptr;
    }

    T * operator->() const // never throws
    {
        return _ptr;
    }

	T * get() const // never throws
    {
        return _ptr;

    }
	operator bool () const
    {
        return _ptr != 0;
    }

	bool operator! () const // never throws
    {
        return _ptr == 0;
    }

	void swap(scoped_ptr & b) // never throws
    {
        T * tmp = b._ptr;
        b._ptr = _ptr;
        _ptr = tmp;
    }

private:
	typedef scoped_ptr<T> this_type;
	T *_ptr;
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

}
#endif //_THEFOX_SCOPED_PTR_H_