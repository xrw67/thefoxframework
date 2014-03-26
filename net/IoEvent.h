#ifndef _THEFOX_NET_IOEVENT_H_
#define _THEFOX_NET_IOEVENT_H_

#include <base/Types.h>

namespace thefox
{

class IoEvent;

/// @brief event type
enum {
	kEventTypeCpRead,
	kEventTypeCpWrite,
	kEventTypeCpZeroByteRead,
	kEventTypeCpClose,
	kEventTypeTimer,
	kEventTypeUser
};

enum  EventError {
	kNoError,
	kCpError
};

typedef void (*EventCallback)(IoEvent *, EventError);

class IoEvent
{
public:
#ifdef WIN32
	OVERLAPPED _overlapped;
#endif
	IoEvent()
		: _bytesTransfered(0)
	{
		// OVERLAPPED需要初始化，否则WSARecv会ERROR_INVALID_HANDLE
		memset(&_overlapped, 0, sizeof(OVERLAPPED));
	}
	int32_t eventType() const { return _eventType; }
	void setEventType(int32_t type) { _eventType = type; }
	void setEventCallback(const EventCallback &cb)
	{ _eventCallback = cb; }
	void setBytesTransfered(DWORD bytesTransfered)
	{ _bytesTransfered = bytesTransfered;}
	void handleEvent(EventError error)
	{ _eventCallback(this, error); }
protected:
	int32_t _eventType;
	EventCallback _eventCallback;
	DWORD _bytesTransfered;
};

} // namespace thefox

#endif // _THEFOX_NET_IOEVENT_H_