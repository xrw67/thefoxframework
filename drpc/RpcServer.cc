#include <rpc/RpcServer.h>
#include <google/protobuf/descriptor.h>

namespace thefox
{
void onRpcConnection(const TcpConnectionPtr &conn)
{
}

void onRpcClose(const TcpConnectionPtr &conn)
{

}

void onRpcMessage(const TcpConnectionPtr &conn, Buffer *buffer, Timestamp recvTime)
{

}

}; // namespace thefox

using namespace thefox;

RpcServer::RpcServer(EventLoop *loop)
	: _server(loop, "thefoxRpcServer")
{
}

RpcServer::~RpcServer()
{
}

void RpcServer::addService(gpb::Service *service)
{
	_services[service->GetDescriptor()->name()] = service;
}

 bool RpcServer::start(const InetAddress &listenAddr)
{
	_server.setConnectionCallback(onRpcConnection);
	
	_server.start(listenAddr);
}

void RpcServer::doneCallback(const int64_t id, const gpb::Message *response)
{
	rpc::Reply reply;
	reply.set_id(id);
	reply.set_result(true);
	reply.set_response(response->SerializeAsString());
}
void RpcServer::onMessage(const TcpConnectionPtr& conn, const rpc::Call *call, Timestamp receiveTime)
{
	ServiceMap::const_iterator it = _services.find(call->service());
	if (it != _services.end()) {
		gpb::Service *service = it->second;
		assert(service != NULL);

		const gpb::ServiceDescriptor *desc = service->GetDescriptor();
		const gpb::MethodDescriptor *method = desc->FindMethodByName(call->method());
		
		if (method) {
			gpb::Message *request(service->GetRequestPrototype(method).New());
			request->ParseFromString(call->request());
			
			const int64_t id = call->id();
			const gpb::Message *response = &service->GetResponsePrototype(method);
			
			service->CallMethod(method, NULL, call, const_cast<gpb::Message*>(response),
				gpb::NewCallback<RpcServer, const int64_t, const gpb::Message *>(this, &RpcServer::doneCallback, call->id(), response));
		}
	}
}