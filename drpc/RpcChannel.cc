#include <rpc/RpcChannel.h>
#include <google/protobuf/descriptor.h>
#include <rpc/rpc.pb.h>

using namespace thefox;

RpcChannel::RpcChannel(const TcpConnectionPtr &conn)
	: _conn(conn)
{
}

RpcChannel::~RpcChannel()
{
}

void RpcChannel::CallMethod(const ::google::protobuf::MethodDescriptor* method,
                            ::google::protobuf::RpcController* controller,
                            const ::google::protobuf::Message* request,
                            ::google::protobuf::Message* response,
                            ::google::protobuf::Closure* done)
{
	rpc::Call call;
	int64_t id = _id.inc();
	call.set_id(id);
	call.set_service(method->service()->name());
	call.set_method(method->name());
	call.set_request(request->SerializeAsString());

	SharedPtr<RequestWait> reqWait(new RequestWait(response, done));
	{
	MutexLockGuard lock(_mutex);
	_requests[id] = reqWait;
	}
	_conn.send(call.SerializeAsString());
	
	if (NULL == done) // synchronize
		reqWait.wait();
}
