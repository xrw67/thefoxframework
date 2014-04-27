#ifndef _THEFOX_RPC_RPCCHANNEL_H_
#define _THEFOX_RPC_RPCCHANNEL_H_

#include <map>
#include <base/Types.h>
#include <base/Event.h>
#include <base/MutexLock.h>
#include <base/AtomicInteger.h>
#include <base/Timestamp.h>
#include <google/protobuf/service.h>
#include <net/TcpClient.h>
#include <rpc/common.h>
#include <rpc/rpc.pb.h>

namespace thefox
{

class RpcChannel : public gpb::RpcChannel
{
public:
  RpcChannel(EventLoop *loop);
  ~RpcChannel();

  bool open(const InetAddress &serverAddr);
  void close();
  void onMessage(const rpc::Reply *reply, Timestamp recvTime);

  void CallMethod(const ::google::protobuf::MethodDescriptor* method,
                 ::google::protobuf::RpcController* controller,
                 const ::google::protobuf::Message* request,
                 ::google::protobuf::Message* response,
                 ::google::protobuf::Closure* done);
private:
	class RequestWait : public Shareable
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

	typedef std::map<int64_t, SharedPtr<RequestWait>> RequestWaitMap;
	AtomicInt64 _id;
	MutexLock _mutex;
	RequestWaitMap _requests;
	TcpClient _client;
};

} // namespace thefox

#endif  // _THEFOX_RPC_RPCCHANNEL_H_
