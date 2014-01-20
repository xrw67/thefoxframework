#ifndef _THEFOX_OBJECT_H_
#define _THEFOX_OBJECT_H_

namespace thefox
{
class Object
{
public:
	void addRef() { ++_refCount; }
	void decRef() { --_refCount; }
	unsigned long getRefCount() const { return _refCount; }
	
private:
	unsigned long _refCount; // 引用计数器
};
}
#endif // _THEFOX_OBJECT_H_