#ifndef _THEFOX_RPC_RPCCHANNEL_H_
#define _THEFOX_RPC_RPCCHANNEL_H_

#include <map>
#include <base/Types.h>
#include <base/Event.h>
#include <base/MutexLock.h>
#include <base/AtomicInteger.h>
#include <google/protobuf/service.h>
#include <net/TcpClient.h>
#include <net/rpc/common.h>
#include <net/rpc/rpc.pb.h>

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
	void setNonRpcMsgCallback(const NonRpcMsgCallback &cb)
	{ _nonRpcMsgCallback = cb; }
private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(RpcChannel);
	void onConnection(const TcpConnectionPtr &conn);
	void onClose(const TcpConnectionPtr &conn);
	void onMessage(const TcpConnectionPtr &conn, Buffer *buf, const Timestamp recvTime);
	void handleReplyMessage(const rpc::Reply &reply, Timestamp recvTime);
	void handleNonRpcMessage(const rpc::NonRpcMsg &nrm, Timestamp recvTime);

	class RequestWait
	{
	public:
		RequestWait(gpb::Message* response, gpb::Closure *done)
			: response(response)
			, done(done)
		{}
		gpb::Message* response;
		gpb::Closure *done;
		Event doneEvent;
	};

	typedef std::shared_ptr<RequestWait> RequestWaitPtr;
	typedef std::map<int64_t, RequestWaitPtr> RequestWaitMap;
	AtomicInt64 _id;
	MutexLock _mutex;
	RequestWaitMap _requests;
	TcpConnectionPtr _conn;
	std::shared_ptr<TcpClient> _client;
	NonRpcMsgCallback _nonRpcMsgCallback;
};

} // namespace thefox

#endif  // _THEFOX_RPC_RPCCHANNEL_H_
