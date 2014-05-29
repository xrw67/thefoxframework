#ifndef _THEFOX_RPC_RPCCHANNEL_H_
#define _THEFOX_RPC_RPCCHANNEL_H_

#include <map>
#include <base/common.h>
#include <google/protobuf/service.h>
#include <net/tcp_client.h>
#include <rpc/common.h>
#include <rpc/rpc.pb.h>

namespace thefox
{

class RpcClient;

class RpcChannel : public gpb::RpcChannel
{
public:
	RpcChannel(RpcClient *rpcClient, const InetAddress &serverAddr);
	~RpcChannel();

	void CallMethod(const ::google::protobuf::MethodDescriptor* method,
				    ::google::protobuf::RpcController* controller,
				   const ::google::protobuf::Message* request,
				   ::google::protobuf::Message* response,
				   ::google::protobuf::Closure* done);

	void setTaskManager(const TaskManagerPtr &taskManager);

private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(RpcChannel);
	void onConnection(TcpConnection *conn);
	void onMessage(TcpConnection *conn, Buffer *buf, Timestamp recvTime);
	
	TcpConnection *_conn;
	std::shared_ptr<TcpClient> _client;
	RpcClient *_rpcClient;
	TaskManagerPtr _taskManager;
};

} // namespace thefox

#endif  // _THEFOX_RPC_RPCCHANNEL_H_
