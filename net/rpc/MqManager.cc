#include <net/rpc/MqManager.h>

namespace thefox {
namespace detail {

void defaultOobCallback(const TcpConnectionPtr &sender, 
							const std::string &type, 
							const gpb::Message *message,
							const Timestamp &receiveTime)
{}

}
}

using namespace thefox;

MqManager::MqManager(const CallCallback &callCb, const ReplyCallback &replyCb)
	: _dispatcher(detail::defaultOobCallback)
	, _callCallback(callCb)
	, _replyCallback(replyCb)
	, _started(false)
	
{
	_msgLoopThread.reset(new Thread(std::bind(&MqManager::loop, this), "mqmanager.loop"));
	_msgLoopThread->start();
}

MqManager::~MqManager()
{
	_started = false;
	_msgLoopThread->stop();
}

void MqManager::pushBox(const TcpConnectionPtr &sender, const Timestamp &receiveTime, const BoxPtr &msg)
{
	MutexLockGuard lock(_mutex);
	MsgBoxPtr box(new MsgBox(sender,receiveTime, msg));
	_msgBoxs.push(box);
	_event.set();
}

MsgBoxPtr MqManager::popBox()
{
	MutexLockGuard lock(_mutex);
	MsgBoxPtr msg(_msgBoxs.front());
	_msgBoxs.pop();
	return msg;
}

void MqManager::setDefaultOobCallback(const OobCallback &cb)
{
	_dispatcher.setDefaultOobCallback(cb);
}

void MqManager::registerOobCallback(const std::string &type, const OobCallback &cb)
{
	_dispatcher.registerOobCallback(type, cb);
}

void MqManager::loop()
{
	while (_started) {
		MsgBoxPtr msg;
		while (_event.wait()) {
			{
			MutexLockGuard lock(_mutex);
			if (_msgBoxs.empty()) {
				_event.reset();
				continue;
			}
			}

			if ((msg = popBox()) != NULL) {
				if (msg->hasCall())
					_callCallback(msg->sender(), msg->call(), msg->time());
				if (msg->hasReply())
					_replyCallback(msg->sender(), msg->reply(), msg->time());
				if (msg->hasOob())
					_dispatcher.onMessage(msg->sender(), msg->oob(), msg->time());
			}
				
		}
	}
}