#ifndef _THEFOX_NET_MSGQUEUE_QUEUETUPLE_H_
#define _THEFOX_NET_MSGQUEUE_QUEUETUPLE_H_

#include <queue>
#include <base/Types.h>
#include <net/msgqueue/Message.h>

namespace thefox
{

namespace mq
{

// not safe
class QueueTuple
{
public:
	void push(std::string *data)
	{
		MutexLockGuard lock(_mutex);
		_msgs.push(new Message(data));
	}

	MessagePtr pop()
	{
		MessagePtr msg(_msgs.front());
		_msgs.front();
		return msg;
	}
	size_t count() const
	{
		return _msgs.size();
	}
private:
	typedef std::queue<MessagePtr> MsgQueue;
	MsgQueue _msgs;
};

typedef std::shared_ptr<QueueTuple> QueueTuplePtr;

} // namespace mq

} // namespace thefox

#endif // _THEFOX_NET_MSGQUEUE_QUEUETUPLE_H_