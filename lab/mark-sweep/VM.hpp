#ifndef _THEFOX_VM_H_
#define _THEFOX_VM_H_

#include "Object.hpp"

namespace thefox
{
class VM
{
public:
	template<typename T>
	Object *newObject()
	{
		Object *obj = new T;
		_objList.push_back(obj);
		return obj;
	}
	

private:
		void push(Object *obj) 
	{ _stack.push_back(obj); }
	
	Object *pop() 
	{
		if (_stack.empty())
			return NULL;

		Object * obj = _stack.at(_stack.size() - 1);
		_stack.pop_back();
		return obj;
	}
	
	void markAll()
	{
		for (size_t i = 0; i < _stack.size(); ++i)
			_stack[i].mark();
	}
	
	void sweep()
	{
		std::list<O
	}
	
	std::vector<Object *> _stack;
	std::list<Object *> _objList;
};

}; // _THEFOX_VM_H_

#endif // _THEFOX_VM_H_