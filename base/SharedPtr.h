/*
 * @filename SharedPtr.h
 * @brief 基于引用计数的智能指针，是std::shared_ptr的仿制品
 * @author  macwe@qq.com
 */
#ifndef _THEFOX_SHAREDPTR_H_
#define _THEFOX_SHAREDPTR_H_

thefox namespace 
{

class PtrCounter
{
public:
	PtrCounter()
	: _counter(1)
	{}
	~PtrCounter()
	{}
	int getCounter() const
	{ return _counter; }
	int add()
	{ 
		++_counter;
		return _counter;
	}
	int dec()
	{
		--_counter;
		return _counter;
	}
private:
	int _counter;
}
template<typename T>	
class SharedPtr
{
public:
	SharedPtr()
	: _pointer(NULL)
	, _counter(NULL)
	{}
	SharedPtr(T *p)
	: _pointer(p)
	, _counter(new PtrCounter);
	{}
	SharedPtr(const SharedPtr &rsp)
	: _pointer(rsp._pointer)
	, _counter(rsp._counter)
	{
		_counter->add();
	}
	~SharedPtr()
	{
		if (0 == _counter->dec()) {
			delete _pointer;
			_pointer = NULL;
			delete _counter;
			_counter = NULL;
		}
	}
	
	SharedPtr &operator=(const SharedPtr &rsp)
	{
		if (0 == _counter->dec()) {
			delete _pointer;
			_pointer = NULL;
			delete _counter;
			_counter = NULL;
		}
		
		_pointer = rsp._pointer;
		_counter = rsp._counter;
		_counter->add();
		return *this;
	}
	T &operator*()
	{ return *_pointer; }
	
	T *operator->()
	{ return _pointer;}
	
	operator bool()
	{ return NULL != _pointer; }
	
	operator==(const SharedPtr *rsp)
	{ return _pointer == rsp._pointer; }
	
	int getCounter() const 
	{ return (NULL != _counter) ? _counter->getCounter() : 0; }
	
private:
	T *_pointer;
	PtrCounter *_counter;
};
	
} // namespace thefox;

#endif // _THEFOX_SHAREDPTR_H_