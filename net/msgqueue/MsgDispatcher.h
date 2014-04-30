#ifndef _THEFOX_NET_MSGQUEUE_MSGDISPATCHER_H_
#define _THEFOX_NET_MSGQUEUE_MSGDISPATCHER_H_

#include <map>
#include <net/msgqueue/common.h>
#include <net/msgqueue/MsgBox.h>

namespace thefox
{

typedef std::function<void(const MsgBoxPtr &box)> MsgBoxCallback;

class MsgDispatcher
{
public:
	explicit MsgDispatcher(const MsgBoxCallback &defaultCallback)
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

	void registerMsgBoxCallback(const gpb::Descriptor *desc, const MsgBoxCallback &cb)
	{ _callbacks[desc] = cb; }

private:
	typedef std::map<const gpb::Descriptor *, MsgBoxCallback> CallbackMap;
	
	CallbackMap _callbacks;
	MsgBoxCallback _defaultCallback;
};

} // namespace thefox

#endif // _THEFOX_NET_MSGQUEUE_MSGDISPATCHER_H_