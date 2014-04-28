#ifndef _THEFOX_NET_MSG_MSGBOX_H_
#define _THEFOX_NET_MSG_MSGBOX_H_

#include <map>
#include <base/Types.h>
#include <net/msg/common.h>
#include <net/msg/MsgBox.h>
#include <google/protobuf/descriptor.h>

namespace thefox
{

typedef std::function<void(const MsgBoxPtr &box)> MessageCallback;

class MsgDispatcher
{
public:
	explicit MsgDispatcher(const MessageCallback &defaultCallback)
		: _defaultCallback(defaultCallback) {}
	~MsgDispatcher(void) {}

	void onMessage(const MsgBoxPtr &msg) const
	{
		CallbackMap::const_iterator it = _callbacks.find(msg.descriptor());
		if (it != _callbacks.end())
			it->second(msg);
		else
			_defaultCallback(msg);
	}

	void registerMessageCallback(const gpb::Descriptor *desc, const MessageCallback &cb)
	{ _callbacks[desc] = cb; }

private:
	typedef std::map<const gpb::Descriptor *, MessageCallback> CallbackMap;
	
	CallbackMap _callbacks;
	MessageCallback _defaultCallback;
};

} // namespace thefox

#endif // _THEFOX_NET_MSG_MSGBOX_H_