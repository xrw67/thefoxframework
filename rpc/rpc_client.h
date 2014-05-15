#ifndef _THEFOX_RPC_RPCCLIENT_H_
#define _THEFOX_RPC_RPCCLIENT_H_

#include <list>
#include <base/types.h>
#include <base/event.h>
#include <base/mutex.h>
#include <base/atomic_integer.h>
#include <net/rpc/common.h>

namespace thefox
{

class EventLoop;
class TaskManager;
class RpcController;

class RpcClient
{
public:
	friend class RpcChannel;

	RpcClient(EventLoop *loop);
	~RpcClient();

	EventLoop *eventloop() { return _loop; }
	
	void registerChannel(RpcChannel *channel);
	void unregisterChannel(RpcChannel *channel);

private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(RpcClient);
	void CallMethod(const TcpConnectionPtr &conn,
					const ::google::protobuf::MethodDescriptor* method,
				    RpcController *controller,
				   const ::google::protobuf::Message* request,
				   ::google::protobuf::Message* response,
				   ::google::protobuf::Closure* done);
	void handleReplyMessage(const TcpConnectionPtr &conn, 
							const rpc::Reply &reply, 
							const Timestamp &recvTime);
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
	typedef std::list<RpcChannel *> RpcChannelList;

	AtomicInt32 _id;
	Mutex _mutex;
	RequestWaitMap _requests;
	EventLoop *_loop;
	RpcChannelList _channels;
	TaskManagerPtr _taskManager;
};

} // namespace thefox

#endif  // _THEFOX_RPC_RPCCLIENT_H_
