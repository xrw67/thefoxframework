#ifndef _THEFOX_THREAD_H_
#define _THEFOX_THREAD_H_

#include <functional>
#include <memory>
#include "afxmt.h"

namespace thefox
{

class Thread :noncopyable
{
public:
    typedef std::function<void()> ThreadFunc;
    
	explicit Thread(const ThreadFunt &cb, const String &name)
        : _cb(cb)
	{
	}

	void start()
	{
		_workStatus = true;
		::AfxBeginThread(ThreadFunc, this);
	}
	
	void stop()
	{
		_workStatus = false;
		::WaitForSingleObject(_endEvent, INFINITE);
	}
    
    bool getWorkStatus() const { return _workStatus; }
priavte:
    static UINT ThreadFunc(LPVOID p)
	{
		Thread *thread = (Thread *)p;
		_cb();
		thread->SetStopFlag();
		return 0;
	}
	
	void setStopFlag() { m_endEvent.SetEvent(); }

    ThreadCallback _cb;
	bool _workStatus;
	CEvent _endEvent;
};

} // namespace thefox

#endif // _THEFOX_THREAD_H_