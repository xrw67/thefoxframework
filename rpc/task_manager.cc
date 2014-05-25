#include <rpc/task_manager.h>

namespace thefox {
namespace detail {

void defaultOnewayCallback(TcpConnection *sender, 
							const std::string &type, 
							const gpb::Message *message,
							const Timestamp &receiveTime)
{}

}
}

using namespace thefox;

TaskManager::TaskManager(const CallCallback &callCb, const ReplyCallback &replyCb)
	: _dispatcher(detail::defaultOnewayCallback)
	, _callCallback(callCb)
	, _replyCallback(replyCb)
	, _started(true)
	
{
	_msgLoopThread.reset(new Thread(std::bind(&TaskManager::loop, this), "mqmanager.loop"));
	_msgLoopThread->start();
}

TaskManager::~TaskManager()
{
	_started = false;
	_msgLoopThread->stop();
}

void TaskManager::pushBox(TcpConnection *sender, const Timestamp &receiveTime, const BoxPtr &msg)
{
	MutexGuard lock(_mutex);
	TaskPtr box(new Task(sender,receiveTime, msg));
	_tasks.push(box);
	_event.set();
}

TaskPtr TaskManager::popBox()
{
	MutexGuard lock(_mutex);
	TaskPtr msg(_tasks.front());
	_tasks.pop();
	return msg;
}

void TaskManager::setDefaultOnewayCallback(const OnewayCallback &cb)
{
	_dispatcher.setDefaultCallback(cb);
}

void TaskManager::registerOnewayCallback(const gpb::Descriptor *desc, const OnewayCallback &cb)
{
	_dispatcher.registerCallback(desc, cb);
}

void TaskManager::loop()
{
	while (_started) {
		TaskPtr msg;
		while (_event.wait()) {
			{
			MutexGuard lock(_mutex);
			if (_tasks.empty()) {
				_event.reset();
				continue;
			}
			}

			if ((msg = popBox()) != NULL) {
				if (msg->hasCall())
					_callCallback(msg->sender(), msg->call(), msg->time());
				if (msg->hasReply())
					_replyCallback(msg->sender(), msg->reply(), msg->time());
				if (msg->hasOneway())
					_dispatcher.onMessage(msg->sender(), msg->oneway(), msg->time());
			}
				
		}
	}
}
