#ifndef _THEFOX_NET_EVENT_H_
#define _THEFOX_NET_EVENT_H_

#include <net/win32.h>

namespace thefox 
{
namespace net
{
  
class TcpConnection;

class Event
{
public:
	OVERLAPPED _overlapped;

	enum {
		EVENT_READ = 1,
        EVENT_READ_COMPLETE = 2,
		EVENT_WRITE = 4,
        EVENT_WRITE_COMPLETE = 8,
        EVENT_ZERO_BYTE_READ = 16,
        EVENT_ZERO_BYTE_READ_COMPLETE = 32,
		EVENT_CLOSE = 64
	};

	Event();
	~Event();

	void setEvent(int evt) { _event = evt; }
	int getEvent() const { return _event; }
    void setData(void *data) { _data = data; }
    void *getData() const { return _data; }
    
	void handleEvent();
private:
    TcpConnection *_conn;
	int _event;
    void *_data;
};

} // namespace net
} // namespace thefox

#endif // _THEFOX_NET_EVENT_H_