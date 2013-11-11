#ifndef _THEFOX_BASE_EVETN_H_
#define _THEFOX_BASE_EVETN_H_

#include <windows.h>
#include <base/noncopyable.h>

namespace thefox
{

class Event : noncopyable
{
public:
	Event()
		: _event(NULL)
	{
		_event = ::CreateEvent(NULL, TRUE, FALSE, NULL);
		if (NULL == _event)
		{
			// 
		}
	}
	
	~Event()
	{
		if (NULL != _event)
		{
			::CloseHandle(_event);
		}
	}
	
	void setEvent() { ::SetEvent(_event); }
	
	void resetEvent() { ::ResetEvent(_event); }
	
	bool wait()
	{
		::WaitForSingleObject(_event, INFINITE);
		return true;
	}
private:
	HANDLE _event;				
};

}

#endif // _THEFOX_BASE_EVETN_H_