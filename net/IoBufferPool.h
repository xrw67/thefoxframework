#ifndef THEFOX_NET_IO_BUFFER_POOL_H_
#define THEFOX_NET_IO_BUFFER_POOL_H_

#include <base/noncopyable.h>
#include <base/MutexLock.h>
#include <net/IoBuffer.h>

namespace thefox
{

class IoBufferPool : noncopyable
{
public:
	~IoBufferPool()
	{
		MutexLockGuard lock(_lock);
		while (!_bufs.empty()) {
			delete _bufs.front();
			_bufs.pop_front();
		}
	}

	IoBuffer *getBuffer(IoOperation op)
	{
		IoBuffer *buf = NULL;
		MutexLockGuard lock(_lock);
		if (_bufs.empty()) {
			IoBuffer *buf = new IoBuffer(op);
		} else {
			buf = _bufs.front();
			_bufs.pop_front();
			buf->setOperation(op);
		}
		return buf;
	}

	void push(IoBuffer *buf)
	{
		MutexLockGuard lock(_lock);
		_bufs.push_back(buf);
	}

private:
	MutexLock _lock;
	std::list<IoBuffer *> _bufs;

};

} // namespace thefox

#endif // THEFOX_NET_IO_BUFFER_POOL_H_