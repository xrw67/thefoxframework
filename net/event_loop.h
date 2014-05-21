/*
* @filename EventLoop.h
* @brief 事件循环
* @author macwe@qq.com
*/

#ifndef _THEFOX_NET_EVENTLOOP_H_
#define _THEFOX_NET_EVENTLOOP_H_

#include <vector>
#include <Windows.h>
#include <base/types.h>
#include <base/thread.h>

namespace thefox
{

class IoEvent;
class IocpEvent;
class TcpConnection;

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

    void setTimer(uint32_t time, const TimerCallback &cb);

	void addEvent(SOCKET sockfd);
	void postEvent(IoEvent *ev);
	bool delConnection(TcpConnection *conn);

    /// @brief 启动一个工作循环. 警告:单线程,不要使用
    void loop();
private:
    THEFOX_DISALLOW_EVIL_CONSTRUCTORS(EventLoop);
	typedef std::shared_ptr<Thread> ThreadPtr;
    typedef std::vector<ThreadPtr> ThreadVector;

    void init();
#ifdef WIN32
	IocpEvent _poller;
#else
	EpollWvent _poller;
#endif
	bool _started;
    HANDLE _hIocp;
    ThreadVector _threads;
};

} // namespace thefox

#endif // _THEFOX_NET_EVENTLOOP_H_