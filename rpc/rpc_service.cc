#include <rpc/rpc_service.h>
#include <rpc/rpc_service_manager.h>

namespace thefox
{

void defaultHeartBeathCallback()
{}

} // namespace thefox

using namespace thefox;

RpcServiceImpl::RpcServiceImpl(const ServiceManagerPtr &sm)
	: _serviceManager(sm)
	, _heartBeathCallback(defaultHeartBeathCallback)
{}

void RpcServiceImpl::heartBeath(::google::protobuf::RpcController* controller,
                    const ::thefox::rpc::HeartBeath* request,
                    ::thefox::rpc::HeartBeath* response,
                    ::google::protobuf::Closure* done)
{
	_heartBeathCallback();
	done->Run();
}

void RpcServiceImpl::listRpc(::google::protobuf::RpcController* controller,
                    const ::thefox::rpc::Placeholder* request,
                    ::thefox::rpc::RpcList* response,
                    ::google::protobuf::Closure* done)
{
	_serviceManager->getRpcList(response);
	done->Run();
}
