#ifndef _THEFOX_RPC_RPCSERVER_H_
#define _THEFOX_RPC_RPCSERVER_H_

#include <map>
#include <base/Types.h>
#include <base/MutexLock.h>
#include <rpc/common.h>
#include <google/protobuf/service.h>

namespace thefox
{
 
class RpcServiceManager
{
public:
	RpcServiceManager();
	~RpcServiceManager();

	void registerService(gpb::Service *service);
	gpb::Service *findService(const String &serviceName);

private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(RpcServiceManager);
	typedef std::map<String, gpb::Service *> ServiceMap;
	MutexLock _mutex;
	ServiceMap _services;
};

} // namespace thefox

#endif  // _THEFOX_RPC_RPCSERVER_H_

