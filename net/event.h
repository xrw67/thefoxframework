/*
* @filename IoEvent.h
* @brief 表示一个IO事件
* @author macwe@qq.com
*/

#ifndef _THEFOX_NET_EVENT_H_
#define _THEFOX_NET_EVENT_H_

#include <base/types.h>
#include <base/timestamp.h>
#include <base/mutex.h>

namespace thefox
{
class IoEvent {
public:
	IoEvent() {}
	~IoEvent() {}

	TcpConnection *conn;
	uint32_t avaliable;
	int64_t ts; // 时间触发时间
	int32_t ref;
	bool write:1;
	bool read:1;
	Mutex _mutex;
	//*handler;
};

#ifdef WIN32

typedef struct {
	WASOVERLAPPED ovlp;
	Event         *ev;
	int           error;
}EventOverLapped;

#endif

} // namespace thefox

#endif // _THEFOX_NET_EVENT_H_