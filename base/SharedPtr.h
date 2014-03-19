/*
 * @filename SharedPtr.h
 * @brief 基于引用计数的智能指针，是std::shared_ptr的仿制品
 * @author  macwe@qq.com
 */
#ifndef _THEFOX_SHAREDPTR_H_
#define _THEFOX_SHAREDPTR_H_

namespace thefox  
{

template<typename T> class SharedPtr;
template<typename T> class WeakPtr;

class PtrCounter
{
public:
	friend template<typename T> class SharedPtr;
	friend template<typename T> class WeakPtr;
	
	PtrCounter()
	: _strongCounter(1)
	, _weakCounter(0)
	{}
	~PtrCounter()
	{}

private:
	int _strongCounter;
	int _weakCounter;
};

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
	, _counter(new PtrCounter)
	{}
	SharedPtr(const SharedPtr &rsp)
	: _pointer(rsp._pointer)
	, _counter(rsp._counter)
	{
		++_counter->_strongCounter;
	}
	~SharedPtr()
	{
		if (_counter) {
			--_counter->_strongCounter;
			if (0 == _counter->_strongCounter) {
				delete _pointer;
				_pointer = NULL;
				delete _counter;
				_counter = NULL;
			}
		}
	}
	
	SharedPtr &operator=(const SharedPtr &rsp)
	{
		if (_counter) {
			--_counter->_strongCounter;
			if (0 == _counter->_strongCounter) {
				delete _pointer;
				_pointer = NULL;
				delete _counter;
				_counter = NULL;
			}
		}
		
		_pointer = rsp._pointer;
		_counter = rsp._counter;
		++_counter->_strongCounter;
		return *this;
	}
	T &operator*()
	{ return *_pointer; }
	
	T *operator->()
	{ return _pointer;}
	
	operator bool()
	{ return NULL != _pointer; }
	
	bool operator==(const SharedPtr &rsp)
	{ return _pointer == rsp._pointer; }
	
	int getCounter() const 
	{ return (NULL != _counter) ? _counter->_strongCounter : 0; }
	
private:
	T *_pointer;
	PtrCounter *_counter;
};
	
} // namespace thefox;

#endif // _THEFOX_SHAREDPTR_H_