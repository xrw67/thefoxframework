#include <net/rpc/RpcClient.h>
#include <net/TcpConnection.h>
#include <net/rpc/RpcChannel.h>
#include <net/rpc/TaskManager.h>

using namespace thefox;

RpcClient::RpcClient(EventLoop *loop)
	: _loop(loop)
{
	_taskManager = std::make_shared<TaskManager>(
		NULL, std::bind(&RpcClient::handleReplyMessage, this, _1, _2, _3));
}

RpcClient::~RpcClient() 
{}

void RpcClient::registerChannel(RpcChannel *channel)
{
	channel->setTaskManager(_taskManager);
	_channels.push_back(channel);
}

void RpcClient::unregisterChannel(RpcChannel *channel)
{
	_channels.remove(channel);
}

void RpcClient::CallMethod(const TcpConnectionPtr &conn,
					const ::google::protobuf::MethodDescriptor* method,
				    ::google::protobuf::RpcController* controller,
				   const ::google::protobuf::Message* request,
				   ::google::protobuf::Message* response,
				   ::google::protobuf::Closure* done)
{
	rpc::Call *call = new rpc::Call();

	int64_t id = _id.inc();
	call->set_id(id);
	call->set_service(method->service()->name());
	call->set_method(method->name());
	call->set_request(request->SerializeAsString());

	RequestWaitPtr reqWait(new RequestWait(response, done));
	
	{
	MutexLockGuard lock(_mutex);
	_requests[id] = reqWait;
	}

	rpc::Box box;
	box.set_allocated_call(call);
	conn->send(RpcCodec::encode(box));
	
	if (NULL == done) // synchronize
		reqWait->doneEvent.wait();
}

void RpcClient::handleReplyMessage(const TcpConnectionPtr &conn, const rpc::Reply &reply, const Timestamp &recvTime)
{
	const int64_t id = reply.id();

	RequestWaitPtr reqWait;

	{
	MutexLockGuard lock(_mutex);
	RequestWaitMap::iterator it = _requests.find(id);
	if (it != _requests.end()) {
		reqWait = it->second;
		_requests.erase(it);
	}
	}

	if (reqWait) {
		reqWait->response->ParseFromString(reply.response());
		reqWait->doneEvent.set();
		if (reqWait->done)
			reqWait->done->Run();
	}
}