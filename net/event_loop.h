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

    /// @brief 注册句柄到时间循环上
    /// @param[in] handle 句柄变量
    void registerHandle(HANDLE handle);

    /// @brief 投递事件到消息循环上
    /// @param[in] 投递的事件指针
    void postEvent(IoEvent *e);

    /// @brief 启动一个工作循环. 警告:单线程,不要使用
    void loop();
private:
    THEFOX_DISALLOW_EVIL_CONSTRUCTORS(EventLoop);
	typedef std::shared_ptr<Thread> ThreadPtr;
    typedef std::vector<ThreadPtr> ThreadVector;

    void init();
    int getCpuNum();
	bool _started;
    HANDLE _hIocp;
    ThreadVector _threads;
};

} // namespace thefox

#endif // _THEFOX_NET_EVENTLOOP_H_