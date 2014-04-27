#ifndef _THEFOX_RPC_RPCCLIENT_H_
#define _THEFOX_RPC_RPCCLIENT_H_

#include <base/Types.h>
#include <net/TcpClient.h>
#include <rpc/common.h>
#include <rpc/RpcServiceNamager.h>
#include <google/protobuf/service.h>

namespace thefox
{

class RpcClient
{
public:
  RpcClient(EventLoop *loop);
  ~RpcClient();

  void registerService(gpb::Service *service);
  bool open(const InetAddress &serverAddr);
  void close();

private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(RpcClient);
	TcpClient _client;
	RpcServiceManager _servicesManager;
};

} // namespace thefox

#endif  // _THEFOX_RPC_RPCCLIENT_H_
