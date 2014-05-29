/*
* @filename event_loop.h
* @brief 事件循环
* @author macwe@qq.com
*/

#ifndef _THEFOX_NET_EVENTLOOP_H_
#define _THEFOX_NET_EVENTLOOP_H_

#include <vector>
#include <base/common.h>

namespace thefox
{

class Thread;
class IoEvent;
class TcpConnection;

#ifdef WIN32
class IocpEvent;
typedef void *HANDLE;
#else
class EpollEvent;
#endif

/// @brief 事件循环类
class EventLoop
{
public:
    EventLoop();
    ~EventLoop();

    /// @brief 开始消息循环(多线程)
    void start();

    /// @brief 阻塞并等待时间循环退出
    void join();

    /// @brief 退出消息循环
    void stop();

    //void setTimer(uint32_t time, const TimerCallback &cb);

	bool addEvent(IoEvent *ev);
	bool postClose(IoEvent *ev);
	bool updateRead(IoEvent *ev);
	bool updateWrite(IoEvent *ev);
	bool delConnection(TcpConnection *conn);

    /// @brief 启动一个工作循环. 警告:单线程,不要使用
    void loop();
private:
    THEFOX_DISALLOW_EVIL_CONSTRUCTORS(EventLoop);

    void init();
	int getCpuNumber();

	bool _started;
    std::vector<Thread *> _threads;

#ifdef WIN32
	IocpEvent *_poller;
#else
	EpollWvent *_poller;
#endif
};

} // namespace thefox

#endif // _THEFOX_NET_EVENTLOOP_H_