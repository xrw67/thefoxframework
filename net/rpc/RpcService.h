#ifndef _THEFOX_NET_RPC_RPCSERVICE_H_
#define _THEFOX_NET_RPC_RPCSERVICE_H_

#include <base/Types.h>
#include <net/rpc/common.h>
#include <net/rpc/rpc.pb.h>

namespace thefox {

class RpcServiceImpl : public rpc::RpcService
{
public:
	RpcServiceImpl(const ServiceManagerPtr &sm);

	void heartBeath(::google::protobuf::RpcController* controller,
                       const ::thefox::rpc::HeartBeath* request,
                       ::thefox::rpc::HeartBeath* response,
                       ::google::protobuf::Closure* done);

	void listRpc(::google::protobuf::RpcController* controller,
                       const ::thefox::rpc::Placeholder* request,
                       ::thefox::rpc::RpcList* response,
                       ::google::protobuf::Closure* done);

	void setHearthBeathCallback(const HeartBeathCallback &cb)
	{ _heartBeathCallback = cb; }

private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(RpcServiceImpl);
	ServiceManagerPtr _serviceManager;
	HeartBeathCallback _heartBeathCallback;
};

} // namespace thefox

#endif // _THEFOX_NET_RPC_RPCSERVICE_H_