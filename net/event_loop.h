/*
* @brief 事件循环
* @author macwe1024 at gmail dot com
*/

#ifndef _THEFOX_NET_EVENTLOOP_H_
#define _THEFOX_NET_EVENTLOOP_H_

#include <vector>
#include <base/common.h>

namespace thefox
{

class Thread;
class TcpConnection;

#ifdef WIN32
    class IocpEvent;
    typedef IocpEvent Poller;
    typedef void *HANDLE;
#else
    class EpollEvent;
    typedef EpollEvent Poller;
#endif

class EventLoop
{
public:
    EventLoop();
    ~EventLoop();

    // for user

    /// @brief start event loop
    void start();

    /// @brief blocking and wait thread quit
    void join();

    /// @brief quit thread
    void stop();

    //void setTimer(uint32_t time, const TimerCallback &cb);

    // for internal
    bool updateRead(TcpConnection *conn);
    bool updateWrite(TcpConnection *conn);
    bool registerConnection(TcpConnection *conn);
    bool unregisterConnection(TcpConnection *conn);

private:
    THEFOX_DISALLOW_EVIL_CONSTRUCTORS(EventLoop);

    void loop();

    Poller *_poller;
    bool _started;
    const int _threadNum; // number of threads
    std::vector<Thread *> _threads;
};

} // namespace thefox

#endif // _THEFOX_NET_EVENTLOOP_H_