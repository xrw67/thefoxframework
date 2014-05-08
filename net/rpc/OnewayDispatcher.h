#ifndef _THEFOX_NET_RPC_ONEWAYDISPATCHER_H_
#define _THEFOX_NET_RPC_ONEWAYDISPATCHER_H_

#include <map>
#include <base/Types.h>
#include <net/rpc/common.h>
#include <net/rpc/Task.h>
#include <net/rpc/RpcCodec.h>
#include <net/rpc/rpc.pb.h>

namespace thefox
{

class OnewayDispatcher
{
public:
	explicit OnewayDispatcher(const OnewayCallback &defaultCb)
		: _defaultCallback(defaultCb) {}
	~OnewayDispatcher(void) {}

	void onMessage(const ConnectionPtr &sender, const rpc::OnewayMessage &oneway, const Timestamp &recvTime) const
	{
		std::shared_ptr<gpb::Message> message(RpcCodec::createMessage(oneway.type()));
		if (NULL == message) // oneway消息创建失败
			return;

		message->ParseFromString(oneway.body());

		CallbackMap::const_iterator it = _callbacks.find(oneway.type());
		if (it != _callbacks.end())
			it->second(sender, oneway.type(), message.get(), recvTime);
		else
			_defaultCallback(sender, oneway.type(), message.get(), recvTime);
	}

	void setDefaultOobCallback(const OnewayCallback &cb)
	{ _defaultCallback = cb; }

	void registerOobCallback(const std::string type, const OnewayCallback &cb)
	{ _callbacks[type] = cb; }

private:
	typedef std::map<const std::string, OnewayCallback> CallbackMap;
	CallbackMap _callbacks;
	OnewayCallback _defaultCallback;
};

} // namespace thefox

#endif // _THEFOX_NET_RPC_ONEWAYDISPATCHER_H_