#ifndef _THEFOX_RPC_RPCSERVER_H_
#define _THEFOX_RPC_RPCSERVER_H_

#include <map>
#include <base/Types.h>
#include <net/TcpServer.h>
#include <rpc/common.h>
#include <google/protobuf/service.h>

namespace thefox
{

class RpcServer
{
public:
	RpcServer(EventLoop *loop);
	~RpcServer();

	void addService(gpb::Service *service);
	bool start(const InetAddress &listen);
	
	void onMessage(const TcpConnectionPtr& conn, const rpc::Call *call, Timestamp receiveTime);

	void doneCallback(const int64_t id, const gpb::Message *response);
private:
	typedef std::map<String, gpb::Service *> ServiceMap;
	TcpServer _server;
	ServiceMap _services;
};

} // namespace thefox

#endif  // _THEFOX_RPC_RPCSERVER_H_

