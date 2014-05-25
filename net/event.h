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
	TcpConnection *conn;
	uint32_t avaliable;
	int64_t ts; // 时间触发时间
	bool read;
	bool write;
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

#ifdef WIN32

#define OVLP_TYPE_NONE        0
#define OVLP_TYPE_READ        1
#define OVLP_TYPE_WRITE       2
#define OVLP_TYPE_CLOSE       3

typedef struct {
	OVERLAPPED ovlp;
	int32_t type;
	IoEvent *ev;
} EventOvlp;

#endif

} // namespace thefox

#endif // _THEFOX_NET_EVENT_H_