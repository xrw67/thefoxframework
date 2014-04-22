#include <rpc/RpcServer.h>
#include <net/TcpServer.h>
#include <rpc/RpcCodec.h>
#include <google/protobuf/descriptor.h>

namespace thefox
{

static RpcServer *rpcServerPointer;
    
void onRpcConnection(const TcpConnectionPtr &conn)
{
}

void onRpcClose(const TcpConnectionPtr &conn)
{

}

void onRpcMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestamp recvTime)
{
    while (buf->) {
        
    }
    rpcServerPointer
    
    delete call;
}

}; // namespace thefox

using namespace thefox;

RpcServer::RpcServer(EventLoop *loop)
	: _server(new TcpServer(loop, "thefoxRpcServer")
{
    rpcServerPointer = this;
}

RpcServer::~RpcServer()
{
    MutexLockGuard lock(_mutex);
    for (ServiceMap::iterator it = _services.begin();
         it != _services.end(); ++it) {
        delete it->second;
    }
    _services.clear();
}

void RpcServer::registerService(gpb::Service *service)
{
	_services[service->GetDescriptor()->name()] = service;
}

 bool RpcServer::start(const InetAddress &listenAddr)
{
	_server->setConnectionCallback(onRpcConnection);
	
	_server->start(listenAddr);
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
			
			gpb::Message *response = &service->GetResponsePrototype(method).New());
            
			service->CallMethod(method, NULL, call, response, gpb::NewCallback(&DoNothing));
            
            //done
            rpc::Reply reply;
            reply.set_id(call->id());
            reply.set_result(true);
            reply.set_response(response->SerializeAsString());
            _server.send(conn,rpcEncode(reply));
		}
	}
}