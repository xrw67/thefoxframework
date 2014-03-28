/*
* @filename EventLoop.h
* @brief 事件循环
* @author macwe@qq.com
*/

#ifndef _THEFOX_NET_EVENTLOOP_H_
#define _THEFOX_NET_EVENTLOOP_H_

#include <Windows.h>

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
	void quit();

	/// @brief 注册句柄到时间循环上
	/// @param[in] handle 句柄变量
	void registerHandle(HANDLE handle);

	/// @brief 投递事件到消息循环上
	/// @param[in] 投递的事件指针
	void postEvent(IoEvent *e);

	/// @brief 启动一个工作循环. 警告:单线程,不要使用
	void loop();
private:
	void init();
	int getCpuNum();
	HANDLE _hIocp;
	HANDLE _hQuitEvent;
};

} // namespace thefox

#endif // _THEFOX_NET_EVENTLOOP_H_