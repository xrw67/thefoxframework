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

typedef void (*EventCallback)(IoEvent *);

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
	void setEventCallback(const EventCallback &eventCb, const EventCallback &errorCb)
	{ 
		_eventCallback = eventCb;
		_errorCallback = errorCb;
	}
	void setBytesTransfered(DWORD bytesTransfered)
	{ _bytesTransfered = bytesTransfered;}
	DWORD bytesTransfered() const { return _bytesTransfered; }
	void handleEvent()
	{ _eventCallback(this); }
	void handleError()
	{ _errorCallback(this); }

protected:
	int32_t _eventType;
	EventCallback _eventCallback;
	EventCallback _errorCallback;
	DWORD _bytesTransfered;
};

} // namespace thefox

#endif // _THEFOX_NET_IOEVENT_H_