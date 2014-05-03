#ifndef _THEFOX_NET_MSGQUEUE_QUEUETUPLE_H_
#define _THEFOX_NET_MSGQUEUE_QUEUETUPLE_H_

#include <queue>
#include <base/Types.h>
#include <base/MutexLock.h>
#include <net/msgqueue/MsgBox.h>

namespace thefox
{

/// @berief 表示一个消息队列 not safe
class QueueTuple
{
public:
	QueueTuple(const std::string &name)
		:_name(name) 
	{}
	~QueueTuple()
	{}
	
	void push(const TcpConnectionPtr &sender, gpb::Message *msg)
	{
		MsgBoxPtr box(new MsgBox(sender, msg));
		_msgs.push(box);
	}

	MsgBoxPtr pop()
	{
		MsgBoxPtr box(_msgs.front());
		_msgs.front();
		return box;
	}
	size_t size() const
	{
		return _msgs.size();
	}
private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(QueueTuple);
	typedef std::queue<MsgBoxPtr> MsgQueue;
	MsgQueue _msgs;
	const std::string _name;
};

typedef std::shared_ptr<QueueTuple> QueueTuplePtr;

} // namespace thefox

#endif // _THEFOX_NET_MSGQUEUE_QUEUETUPLE_H_