#ifdef _THEFOX_OBJECT_H_
#define _THEFOX_OBJECT_H_

namespace thefox
{

typedef Event;

class Object
{
public:
	Object()
	~Object();
	
	// 引用计数支持
	void addRef()
	{
		MutexLockGuard lock(_mutex);
		++_refCount
	}
	int32_t release()
	{
		MutexLockGuard lock(_mutex);
		if (_refCount > 0)
			--_refCount;
		return _refCount;
	}
	int32_t getCount()
	{
		MutexLockGuard lock(_mutex);
		return _refCount;
	}
private:
	int32_t _refCount; // 
	MutexLock _mutex;

// 事件支持
public:
	virtual void event(Event *e)
	{}
};

} // namespace

#endif // _THEFOX_OBJECT_H_