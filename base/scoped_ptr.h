#ifndef _THEFOX_SCOPED_PTR_H_
#define _THEFOX_SCOPED_PTR_H_

namespace thefox
{

template<T t>
class scoped_ptr
{
public:
	scoped_ptr()
		: _t(NULL);
	{
	}
	~scoped_ptr()
	{
		if (_t)
		{
			delete _t;
			_t = NULL;
		}
	}

	T *operator->()
	{
		return this;
	}
private:
	T *_t;
};

}
#endif //_THEFOX_SCOPED_PTR_H_