#ifndef _THEFOX_BASE_THREAD_H_
#define _THEFOX_BASE_THREAD_H_

#include <vector>
#include <base/common.h>

#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
#else
    #include <pthread.h>
#endif

namespace thefox
{

typedef std::function<void()> ThreadCallback;

class Thread
{
public:
    enum StateT { kInit, kStart, kJoined, kStop };

    explicit Thread(const ThreadCallback &cb)
        : _cb(cb)
        , _state(kInit)
    #ifdef WIN32
        , _handle(NULL)
        , _threadId(0)
    #endif
    {}

    ~Thread()
    {
        join();
        _state = kStop;
    }

    bool start()
    {
        if (kInit != _state) {
            return false;
        }

        bool result = false;
#ifdef WIN32
        _handle = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadProc, (LPVOID)this, 0, &_threadId);
        result = (NULL != _handle);
#else
        int ret = pthread_create(&_thread, NULL, threadProc, (void *)this);
        result = (0 == ret);
#endif
        _state = kStart;
        return result;
    }
    
    bool stop()
    {
        if (kStop == _state || kInit == _state) {
            return true;
        }

        bool result = true;
#ifdef WIN32
        if(0 == ::TerminateThread(_handle, 0)) {
            result = false;
        }
#else
        if (0 != pthread_cancel(_thread)) {
            result = false;
        }
#endif
        if (result) {
            _state = kStop;
        }

        return result;
    }

    bool join()
    {
        if (kStart != _state) {
            return false;
        }
        bool result = false;
#ifdef WIN32
        if (NULL != _handle) {
            DWORD ret = ::WaitForSingleObject(_handle, INFINITE);
            if (WAIT_OBJECT_0 == ret || WAIT_ABANDONED == ret) {
                result = true;
                _handle = NULL;
            }
        }
#else
        int ret = pthread_join(_thread, NULL);
        if (0 == ret) {
            result = true;
        }
#endif
        _state = kJoined;
        return result;
    }

#ifdef WIN32
    DWORD tid() const { return _threadId; }
    operator HANDLE() { return _handle; }
#else
    pthread_t tid() const { return _thread; }
#endif

private:
    THEFOX_DISALLOW_EVIL_CONSTRUCTORS(Thread);

#ifdef WIN32
    static DWORD WINAPI threadProc(LPVOID param)
#else
	static void *threadProc(void *param)
#endif
    {
        Thread *pThis = reinterpret_cast<Thread *>(param);
        pThis->_cb();
		return 0;
    }

    ThreadCallback _cb;
    StateT _state;

#ifdef _WIN32
    HANDLE _handle;
    DWORD _threadId;
#else
    pthread_t _thread;
#endif
};

/// @breaf Ïß³Ì×é
class ThreadGroup
{
public:
	ThreadGroup()
	{}
	~ThreadGroup()
	{
		for (size_t i = 0; i < _threads.size(); ++i) {
			delete _threads[i];
		}
		
		_thread.clear();
	}
	
	Thread *createThread(const ThreadCallback &threadfunc)
	{
		Thread *thread = new Thread(threadfunc);
		_threads.push_back(thread);
		return thread;
	}
	
	void addThread(Thread *thread) 
	{
		_threads.push_back(thread);
	}
	
	void joinAll()
	{
		for (size_t i = 0; i < _threads.size(); ++i) {
			_threads[i].join();
		}
	}
	
	size_t size() const
	{ 
		return _threads.size(); 
	}
	
private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(ThreadGroup);
	std::vector<thread *> _threads;
};

} // namespace thefox

#endif // _THEFOX_BASE_THREAD_H_
