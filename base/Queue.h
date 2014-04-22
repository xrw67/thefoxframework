/*
 * @filename Queue.h
 * @brief 线程安全的队列
 * @author macwe@qq.com
 */

#ifndef _THEFOX_BASE_QUEUE_H_
#define _THEFOX_BASE_QUEUE_H_

namespace thefox
{

template<typename T>
class Queue
{
public:
	void push(T &data)
	{
		MutexLockGuard lock(_mutex);
	}

	T &pop()
	{
	}
	size_t size()
	{}
private:
	struct Item
	{
		struct Item *next;
		T data;
	};

	struct Item *_head;
	struct Item *_foot;
	size_t count;
	MutexLock _mutex;
};

} // namespace thefox

#endif // _THEFOX_BASE_QUEUE_H_
