/*
* @brief a io event entry
* @author macwe1024 at gmail dot com
*/

#ifndef _THEFOX_NET_EVENT_H_
#define _THEFOX_NET_EVENT_H_

#ifdef WIN32
#include <WinBase.h>
#endif

#include <base/common.h>
#include <base/mutex.h>
#include <net/socket.h>

namespace thefox {
namespace net {

class TcpConnection;
class Event;

class Event
{
public:
	typedef std::function<void(Event *, void *arg)>  Handler;

	Event()
        : in(false), out(false), error(false), fd(INVALID_SOCKET), conn(NULL)
	{}

	bool testAndSetWrite()
	{
	    MutexGuard lock(_mutex);
        bool temp = out;
        out = true;
	    return temp;
	}

	void resetWrite()
	{
	    MutexGuard lock(_mutex);
	    out = false;
	}

	bool in;
	bool out;
	bool error;

	SOCKET fd;
	TcpConnection *conn;

	Handler handler;
private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(Event);
	Mutex _mutex;
};


#ifdef WIN32

    #define OVLP_TYPE_READ        1
    #define OVLP_TYPE_WRITE       2
    #define OVLP_TYPE_CLOSE       3

    typedef struct {
	    OVERLAPPED ovlp;
	    int32_t type; // io类型
        uint32_t avaliable; // 传输的字节数
	    Event *ev;
    } EventOvlp;

#endif

} // namespace net
} // namespace thefox

#endif // _THEFOX_NET_EVENT_H_
