#ifndef _THEFOX_RPC_RPCCLIENT_H_
#define _THEFOX_RPC_RPCCLIENT_H_

#include <base/Types.h>
#include <net/rpc/common.h>

namespace thefox
{

class EventLoop;

class RpcClient
{
public:
	friend class RpcChannel;

	RpcClient(EventLoop *loop);
	~RpcClient();

	EventLoop *eventloop() { return _loop; }
	void setNonRpcMsgCallback(const NonRpcMsgCallback &cb)
	{ _nonRpcMsgCallback = cb; }
	NonRpcMsgCallback nonRpcMsgCallback() {return _nonRpcMsgCallback; }
private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(RpcClient);
	EventLoop *_loop;
	NonRpcMsgCallback _nonRpcMsgCallback;
};

} // namespace thefox

#endif  // _THEFOX_RPC_RPCCLIENT_H_
