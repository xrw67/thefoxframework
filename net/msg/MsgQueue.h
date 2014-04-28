#ifndef _THEFOX_NET_MSG_MSGQUEUE_H_
#define _THEFOX_NET_MSG_MSGQUEUE_H_

#include <list>
#include <map>
#include <base/Types.h>
#include <base/MutexLock.h>
#include <base/Event.h>
#include <base/Thread.h>
#include <net/msg/common.h>
#include <net/msg/MsgBox.h>

namespace thefox
{

class MsgQueue
{
public:
	class MsgDispatcher;
	typedef std::function<void(const MsgBoxPtr &box)> MsgBoxCallback;

	explicit MsgQueue(const MsgBoxCallback &defaultCallback);
	~MsgQueue();
	
	void putBox(const MsgBoxPtr &box);
	MsgBoxPtr popBox();
	void startloop();
	void loop();
	void registerMsgBoxCallback(const gpb::Descriptor *desc,
								const MsgBoxCallback &cb);
private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(MsgQueue);
	std::unique_ptr<MsgDispatcher> _dispatcher;
	std::list<MsgBoxPtr> _boxs;
	MutexLock _lock;
	Event _msgEvent;
	std::unique_ptr<Thread> _thread;
	bool _started;
};

} // namespace thefox

#endif // _THEFOX_NET_MSG_MSGQUEUE_H_