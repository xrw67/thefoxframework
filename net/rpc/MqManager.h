#ifndef _THEFOX_NET_RPC_MQMANAGER_H_
#define _THEFOX_NET_RPC_MQMANAGER_H_

#include <queue>
#include <base/MutexLock.h>
#include <base/Thread.h>
#include <base/Event.h>
#include <net/rpc/common.h>
#include <net/rpc/MsgBox.h>
#include <net/rpc/OobDispatcher.h>

namespace thefox
{

class OobDispatcher;

class MqManager
{
public:
	MqManager(const CallCallback &callCb, const ReplyCallback &replyCb);
	~MqManager();

	void pushBox(const TcpConnectionPtr &sender, const Timestamp &receiveTime, const BoxPtr &msg);
	MsgBoxPtr popBox();
	void setDefaultOobCallback(const OobCallback &cb);
	void registerOobCallback(const std::string &type, const OobCallback &cb);

private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(MqManager);
	void loop();

	typedef std::queue<MsgBoxPtr> MsgBoxQueue;
	MsgBoxQueue _msgBoxs;
	MutexLock _mutex;
	bool _started;
	Event _event;
	OobDispatcher _dispatcher;
	std::unique_ptr<Thread> _msgLoopThread;

	CallCallback _callCallback;
	ReplyCallback _replyCallback;
};

typedef std::shared_ptr<MqManager> MqManagerPtr;

}// namespace thefox

#endif // _THEFOX_NET_RPC_MQMANAGER_H_
