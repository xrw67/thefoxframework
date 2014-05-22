/*
* @filename IoEvent.h
* @brief 表示一个IO事件
* @author macwe@qq.com
*/

#ifndef _THEFOX_NET_EVENT_H_
#define _THEFOX_NET_EVENT_H_

#ifdef WIN32
#include <WinBase.h>
#endif

#include <base/types.h>
#include <base/timestamp.h>
#include <base/mutex.h>

namespace thefox
{
class TcpConnection;

class IoEvent {
public:
#ifdef WIN32
	OVERLAPPED ovlp;
#endif

	TcpConnection *conn;
	uint32_t avaliable;
	int64_t ts; // 时间触发时间
	bool write;
	bool read;
	bool close;

	void init(TcpConnection *c)
	{
		conn = c;
		avaliable = 0;
		ts = 0;
		write = 0;
		read = 0;
		close = 0;
		_refCount = 0;
	}
	void enterIo() 
	{ 
		MutexGuard lock(_mutex);
		++_refCount;
	}
	int32_t leaveIo()
	{ 
		MutexGuard lock(_mutex);
		return --_refCount;
	}
	uint32_t refCount() const 
	{ return _refCount; }

private:
	Mutex _mutex;
	int32_t _refCount; //引用计数
};

} // namespace thefox

#endif // _THEFOX_NET_EVENT_H_