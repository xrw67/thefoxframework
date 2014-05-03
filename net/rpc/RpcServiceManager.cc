#include <net/rpc/RpcServiceManager.h>
#include <google/protobuf/descriptor.h>
#include <net/rpc/rpc.pb.h>

using namespace thefox;

RpcServiceManager::RpcServiceManager()
{
}

RpcServiceManager::~RpcServiceManager()
{
    MutexLockGuard lock(_mutex);
    for (ServiceMap::iterator it = _services.begin(); it != _services.end(); ++it) {
        delete it->second;
    }
    _services.clear();
}

void RpcServiceManager::registerService(gpb::Service *service)
{
	MutexLockGuard lock(_mutex);
	_services[service->GetDescriptor()->name()] = service;
}

gpb::Service *RpcServiceManager::findService(const std::string &serviceName)
{
	MutexLockGuard lock(_mutex);
	ServiceMap::const_iterator it = _services.find(serviceName);
	if (it != _services.end())
		return it->second;

	return NULL;
}

void RpcServiceManager::getRpcList(rpc::RpcList *list)
{
	MutexLockGuard lock(_mutex);
    for (ServiceMap::iterator it = _services.begin(); it != _services.end(); ++it)
        list->add_service(it->first);
}