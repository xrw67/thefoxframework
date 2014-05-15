#include <rpc/rpc_service_manager.h>
#include <google/protobuf/descriptor.h>
#include <rpc/rpc.pb.h>

using namespace thefox;

RpcServiceManager::RpcServiceManager()
{
}

RpcServiceManager::~RpcServiceManager()
{
    MutexGuard lock(_mutex);
    for (ServiceMap::iterator it = _services.begin(); it != _services.end(); ++it) {
        delete it->second;
    }
    _services.clear();
}

void RpcServiceManager::registerService(gpb::Service *service)
{
	MutexGuard lock(_mutex);
	_services[service->GetDescriptor()->name()] = service;
}

gpb::Service *RpcServiceManager::findService(const std::string &serviceName)
{
	MutexGuard lock(_mutex);
	ServiceMap::const_iterator it = _services.find(serviceName);
	if (it != _services.end())
		return it->second;

	return NULL;
}

void RpcServiceManager::getRpcList(rpc::RpcList *list)
{
	MutexGuard lock(_mutex);
    for (ServiceMap::iterator it = _services.begin(); it != _services.end(); ++it)
        list->add_service(it->first);
}