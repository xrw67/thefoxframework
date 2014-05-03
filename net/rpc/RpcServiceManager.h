#ifndef _THEFOX_RPC_RPCSERVICEMANAGER_H_
#define _THEFOX_RPC_RPCSERVICEMANAGER_H_

#include <map>
#include <base/Types.h>
#include <base/MutexLock.h>
#include <net/rpc/common.h>
#include <google/protobuf/service.h>

namespace thefox
{

namespace rpc
{
class RpcList;
}

class RpcServiceManager
{
public:
	RpcServiceManager();
	~RpcServiceManager();

	void registerService(gpb::Service *service);
	gpb::Service *findService(const std::string &serviceName);

	void getRpcList(rpc::RpcList *list);

private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(RpcServiceManager);
	typedef std::map<std::string, gpb::Service *> ServiceMap;

	MutexLock _mutex;
	ServiceMap _services;
};

} // namespace thefox

#endif  // _THEFOX_RPC_RPCSERVICEMANAGER_H_

