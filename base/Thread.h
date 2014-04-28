#ifndef _THEFOX_BASE_THREAD_H_
#define _THEFOX_BASE_THREAD_H_


#include <base/Types.h>

#ifdef WIN32
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

    explicit Thread(const ThreadCallback &cb, void *arg, const std::string &name)
        : _cb(cb)
        , _arg(arg)
        , _name(name)
        , _state(kInit)
    #ifdef WIN32
        , _handle(NULL)
        , _threadId(0)
    #endif
    {}

    ~Thread()
    {
        join();
        setState(kStop);
    }

    bool start()
    {
        if (kInit != state())
            return false;

        bool result = false;
#ifdef WIN32
        _handle = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadProc, (LPVOID)this, 0, &_threadId);
        result = (NULL != _handle);
#else
        int ret = pthread_create(&_thread, NULL, _cb, (void *)_arg);
        result = (0 == ret);
#endif
        setState(kStart);
        return result;
    }
    
    bool stop()
    {
        if (kStop == state() || kInit == state())
            return true;

        bool result = true;
#ifdef WIN32
        if(0 == ::TerminateThread(_handle, 0))
            result = false;
#else
        if (0 != pthread_cancel(_thread))
            result = false;
#endif
        if (result)
            setState(kStop);

        return result;
    }

    bool join()
    {
        if (kStart != state())
            return false;
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
        if (0 == ret)
            result = true;
#endif
        setState(kJoined);
        return result;
    }

    const std::string &name() const { return _name; }
    StateT state() const { return _state; }
	void setState(const StateT &state) { _state = state; }

#ifdef WIN32
    DWORD tid() const { return _threadId; }
    operator HANDLE() { return _handle; }
#else
    pthread_t tid() const
    {}
#endif

private:
    THEFOX_DISALLOW_EVIL_CONSTRUCTORS(Thread);

    void run() { _cb; }

    static DWORD WINAPI threadProc(LPVOID param)
    {
        Thread *pThis = reinterpret_cast<Thread *>(param);
        pThis->run();
		return 0;
    }

    ThreadCallback _cb;
    void *_arg;
    const std::string _name;
    StateT _state;

#ifdef _WIN32
    HANDLE _handle;
    DWORD _threadId;
#else
    pthread_t _thread;
#endif
};

} // namespace thefox

#endif // _THEFOX_BASE_THREAD_H_