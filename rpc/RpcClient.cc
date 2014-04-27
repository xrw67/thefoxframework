#include <rpc/RpcClient.h>
#include <rpc/RpcChannel.h>

namespace thefox
{

using namespace thefox;

RpcClient::RpcClient(EventLoop *loop)
	: _client(loop, "thefox.RpcChannel")
{
	rpcChannelPointer = this;
}

RpcClient::~RpcClient()
{
}

bool RpcClient::open(const InetAddress &serverAddr)
{
	_client.setConnectionCallback(onRpcChannelConnection);
	_client.setMessageCallback(onRpcChannelMessage);
	_client.setCloseCallback(onRpcChannelClose);

	return _client.open(serverAddr);
}

void RpcChannel::close()
{
	_client.close();
}

void RpcChannel::CallMethod(const ::google::protobuf::MethodDescriptor* method,
                            ::google::protobuf::RpcController* controller,
                            const ::google::protobuf::Message* request,
                            ::google::protobuf::Message* response,
                            ::google::protobuf::Closure* done)
{
	rpc::Box box;
	rpc::Call *call = box.add_call();

	int64_t id = _id.inc();
	call->set_id(id);
	call->set_service(method->service()->name());
	call->set_method(method->name());
	call->set_request(request->SerializeAsString());

	SharedPtr<RequestWait> reqWait(new RequestWait(response, done));
	
	{
	MutexLockGuard lock(_mutex);
	_requests[id] = reqWait;
	}

	_client.send(RpcCodec::encode(box));
	
	if (NULL == done) // synchronize
		reqWait->doneEvent.wait();
}

void RpcChannel::onMessage(const rpc::Reply *reply, Timestamp recvTime)
{
	const int64_t id = reply->id();

	SharedPtr<RequestWait> reqWait;

	{
	MutexLockGuard lock(_mutex);
	RequestWaitMap::iterator it = _requests.find(id);
	if (it != _requests.end()) {
		reqWait = it->second;
		_requests.erase(it);
	}
	}

	if (reqWait) {
		reqWait->response->ParseFromString(reply->response());
		reqWait->doneEvent.setEvent();
		if (reqWait->done)
			reqWait->done->Run();
	}
}