#ifndef _THEFOX_NET_RPC_TASKMANAGER_H_
#define _THEFOX_NET_RPC_TASKMANAGER_H_

#include <queue>
#include <base/mutex.h>
#include <base/thread.h>
#include <base/event.h>
#include <net/rpc/common.h>
#include <net/rpc/task.h>
#include <net/rpc/oneway_dispatcher.h>

namespace thefox
{

class OnewayDispatcher;

class TaskManager
{
public:
	TaskManager(const CallCallback &callCb, const ReplyCallback &replyCb);
	~TaskManager();

	void pushBox(const TcpConnectionPtr &sender, const Timestamp &receiveTime, const BoxPtr &msg);
	TaskPtr popBox();
	void setDefaultOnewayCallback(const OnewayCallback &cb);
	void registerOnewayCallback(const gpb::Descriptor *desc, const OnewayCallback &cb);

private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(TaskManager);
	void loop();

	typedef std::queue<TaskPtr> TaskQueue;
	TaskQueue _tasks;
	Mutex _mutex;
	bool _started;
	Event _event;
	OnewayDispatcher _dispatcher;
	std::unique_ptr<Thread> _msgLoopThread;

	CallCallback _callCallback;
	ReplyCallback _replyCallback;
};

typedef std::shared_ptr<TaskManager> TaskManagerPtr;

}// namespace thefox

#endif // _THEFOX_NET_RPC_TASKMANAGER_H_