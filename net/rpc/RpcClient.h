#ifndef _THEFOX_RPC_RPCCLIENT_H_
#define _THEFOX_RPC_RPCCLIENT_H_

#include <base/Types.h>

namespace thefox
{

class EventLoop;

class RpcClient
{
public:
	RpcClient(EventLoop *loop);
	~RpcClient();

	EventLoop *eventloop() { return _loop; }

private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(RpcClient);
	EventLoop *_loop;

};

} // namespace thefox

#endif  // _THEFOX_RPC_RPCCLIENT_H_
