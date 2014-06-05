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

#include <base/common.h>
#include <base/timestamp.h>
#include <base/mutex.h>

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::weak_ptr<TcpConnection> TcpConnectionWeakPtr;
namespace thefox
{
class TcpConnection;


#ifdef WIN32

#define OVLP_TYPE_NONE        0
#define OVLP_TYPE_READ        1
#define OVLP_TYPE_WRITE       2
#define OVLP_TYPE_CLOSE       3

typedef struct {
	OVERLAPPED ovlp;
	int32_t type;
	TcpConnectionWeakPtr conn; // 弱引用
} EventOvlp;

#endif

} // namespace thefox

#endif // _THEFOX_NET_EVENT_H_