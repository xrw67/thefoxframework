#ifndef _THEFOX_RPC_RPCSERVER_H_
#define _THEFOX_RPC_RPCSERVER_H_

#include <base/Types.h>
#include <net/TcpServer.h>
#include <rpc/common.h>
#include <rpc/RpcServiceNamager.h>
#include <google/protobuf/service.h>

namespace thefox
{
 
class RpcServer
{
public:
	RpcServer(EventLoop *loop);
	~RpcServer();

	void registerService(gpb::Service *service);
	bool start(const InetAddress &listen);

private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(RpcServer);
	TcpServer _server;
	RpcServiceManager _servicesManager;
};

} // namespace thefox

#endif  // _THEFOX_RPC_RPCSERVER_H_

