#ifndef _THEFOX_RPC_RPCCHANNEL_H_
#define _THEFOX_RPC_RPCCHANNEL_H_

#include <map>
#include <base/Types.h>
#include <base/Event.h>
#include <base/Shareable.h>
#include <base/SharedPtr.h>
#include <base/MutexLock.h>
#include <base/AtomicInteger.h>
#include <google/protobuf/service.h>
#include <net/TcpConnection.h>

namespace thefox
{

class RpcChannel : public gpb::RpcChannel
{
public:
  RpcChannel(EventLoop *loop);
  ~RpcChannel();
  void CallMethod(const ::google::protobuf::MethodDescriptor* method,
                 ::google::protobuf::RpcController* controller,
                 const ::google::protobuf::Message* request,
                 ::google::protobuf::Message* response,
                 ::google::protobuf::Closure* done);
private:
	class RequestWait : public Shareable
	{
	public:
		RequestWait(const google::protobuf::Message* response, google::protobuf::Closure *done)
			: response(response)
			, done(done)
		{}
		const google::protobuf::Message* response;
		google::protobuf::Closure *done;
		Event doneEvent;
	};

	AtomicInt64 _id;
	MutexLock _mutex;
	std::map<int64_t, SharedPtr<RequestWait>> _requests;
	TcpClient _client;
};

} // namespace thefox

#endif  // _THEFOX_RPC_RPCCHANNEL_H_
