#include <net/msg/MsgQueue.h>

namespace thefox
{

class MsgQueue::MsgDispatcher
{
public:
	explicit MsgDispatcher(const MsgQueue::MsgBoxCallback &defaultCallback)
		: _defaultCallback(defaultCallback) {}
	~MsgDispatcher(void) {}

	void onMessage(const MsgBoxPtr &box) const
	{
		CallbackMap::const_iterator it = _callbacks.find(box->descriptor());
		if (it != _callbacks.end())
			it->second(box);
		else
			_defaultCallback(box);
	}

	void registerMsgBoxCallback(const gpb::Descriptor *desc, const MsgQueue::MsgBoxCallback &cb)
	{ _callbacks[desc] = cb; }

private:
	typedef std::map<const gpb::Descriptor *, MsgQueue::MsgBoxCallback> CallbackMap;
	
	CallbackMap _callbacks;
	MsgQueue::MsgBoxCallback _defaultCallback;
};

} // namespace thefox

using namespace thefox;

MsgQueue::MsgQueue(const MsgBoxCallback& defaultCallback)
	: _dispatcher(new MsgDispatcher(defaultCallback))
	, _started(false)
{
	_thread.reset(new Thread(std::bind(&MsgQueue::loop, this), NULL, "MsgQueueParserThread"));
}

MsgQueue::~MsgQueue()
{
	_started = false;
	_thread->stop();
}

void MsgQueue::putBox(const MsgBoxPtr &msg) 
{ 
	MutexLockGuard lock(_lock);
	_boxs.push_back(msg); 
	_msgEvent.set();
}

MsgBoxPtr MsgQueue::popBox()
{
	MutexLockGuard lock(_lock);
	if (_boxs.empty()) {
		return NULL;
	}
	MsgBoxPtr box = _boxs.front();
	_boxs.pop_front();
	return box;
}

void MsgQueue::registerMsgBoxCallback(const google::protobuf::Descriptor *desc, 
							 const MsgBoxCallback &cb)
{
	_dispatcher->registerMsgBoxCallback(desc, cb);
}

void MsgQueue::startloop()
{
	_started = true;
	_thread->start();
}

void MsgQueue::loop()
{
	while (_started) {
		MsgBoxPtr msg;
		while (WAIT_OBJECT_0 == WaitForSingleObject(_msgEvent, INFINITE)) {
			{
			MutexLockGuard lock(_lock);
			if (_boxs.empty()) {
				_msgEvent.reset();
				continue;
			}
			}

			if ((msg = popBox()) != NULL)
				_dispatcher->onMessage(msg);
		}
	}
}
