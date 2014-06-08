/*
* @filename IoEvent.h
* @brief 表示一个IO事件
* @author macwe1024 at gmail dot com
*/

#ifndef _THEFOX_NET_EVENT_H_
#define _THEFOX_NET_EVENT_H_

#ifdef WIN32
#include <WinBase.h>
#endif

#include <base/common.h>

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
	    TcpConnection *conn;
    } EventOvlp;

#endif

} // namespace thefox

#endif // _THEFOX_NET_EVENT_H_