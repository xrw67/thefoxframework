#ifndef _THEFOX_NET_RPC_OOBDISPATCHER_H_
#define _THEFOX_NET_RPC_OOBDISPATCHER_H_

#include <map>
#include <base/Types.h>
#include <net/rpc/common.h>
#include <net/rpc/MsgBox.h>
#include <net/rpc/RpcCodec.h>
#include <net/rpc/rpc.pb.h>

namespace thefox
{

class OobDispatcher
{
public:
	explicit OobDispatcher(const OobCallback &defaultCb)
		: _defaultCallback(defaultCb) {}
	~OobDispatcher(void) {}

	void onMessage(const TcpConnectionPtr &sender, const rpc::OutOfBand &oob, const Timestamp &recvTime) const
	{
		std::shared_ptr<gpb::Message> message(RpcCodec::createMessage(oob.type()));
		if (NULL == message) // oob消息创建失败
			return;

		message->ParseFromString(oob.body());

		CallbackMap::const_iterator it = _callbacks.find(oob.type());
		if (it != _callbacks.end())
			it->second(sender, oob.type(), message.get(), recvTime);
		else
			_defaultCallback(sender, oob.type(), message.get(), recvTime);
	}

	void setDefaultOobCallback(const OobCallback &cb)
	{ _defaultCallback = cb; }

	void registerOobCallback(const std::string type, const OobCallback &cb)
	{ _callbacks[type] = cb; }

private:
	typedef std::map<const std::string, OobCallback> CallbackMap;
	CallbackMap _callbacks;
	OobCallback _defaultCallback;
};

} // namespace thefox

#endif // _THEFOX_NET_RPC_OOBDISPATCHER_H_