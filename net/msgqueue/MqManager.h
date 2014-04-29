#ifndef _THEFOX_NET_MSGQUEUE_MQMANAGER_H_
#define _THEFOX_NET_MSGQUEUE_MQMANAGER_H_

#include <map>
#include <base/Types.h>
#include <base/MutexLock.h>
#include <net/msgqueue/Message.h>
#include <net/msgqueue/QueueTuple.h>

namespace thefox
{

namespace mq
{

class MqManager
{
public:
	MqMaager()
	{}
	~MqManager()
	{}

	void pushMsg(const std::string &queueName, std::string *msg)
	{
		MutexLockGuard lock(_mutex);
		QueueTupleMap::iterator it = _queues.find(queueName);
		if (it != _queues.end()) {
			it->second->push(msg);
		} else {
			QueueTuplePtr queue(new QueueTuple());
			queue->push(msg);
			_queues[queueName] = queue;
		}
	}

	MessagePtr popMsg(const std::string &queueName)
	{
		MutexLockGuard lock(_mutex);
		QueueTupleMap::iterator it = _queues.find(queueName);
		if (it != _queues.end()) {
			return it->second->pop(msg);
		} else {
			return NULL;
		}
	}

	// not safe
	QueueTuplePtr findQueue(const std::string &queueName)
	{
		QueueTupleMap::iterator it = _queues.find(queueName);
		if (it != _queues.end()) {
			return it->second;
		} else {
			return NULL;
		}
	}

private:
	// <queueName, queueTuple>
	typedef std::map<std::string, QueueTuplePtr> QueueTupleMap;
	QueueTupleMap _queues;
	MutexLock _mutex;
};

} // namespace mq

}// namespace thefox

#endif // _THEFOX_NET_MSGQUEUE_MQMANAGER_H_
