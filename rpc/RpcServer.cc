#include <rpc/RpcServer.h>
#include <net/Buffer.h>
#include <net/TcpConnection.h>
#include <rpc/RpcCodec.h>
#include <rpc/RpcChannel.h>

namespace thefox
{

static RpcServer *rpcServerPointer = NULL; // 全局的rpc服务器指针
    
void onRpcServerConnection(const TcpConnectionPtr &conn)
{
	conn->bindRpcChannel(new RpcChannel(conn));
}

void onRpcServerClose(const TcpConnectionPtr &conn)
{

}

void onRpcServerMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestamp recvTime)
{
	assert (NULL != rpcServerPointer);

	while (RpcCodec::isValid(buf->peek(), buf->readableBytes())) {
		rpc::Box box;
		size_t bufLen = buf->readableBytes();
		if (RpcCodec::parseFromArray(buf->peek(), bufLen, &box)) {
			for (int i = 0; i < box.call_size(); ++i)
				rpcServerPointer->onMessage(conn, &box.call(i), Timestamp(Timestamp::now()));
			buf->retrieve(bufLen);
		}
	}
}

}; // namespace thefox

using namespace thefox;

RpcServer::RpcServer(EventLoop *loop)
	: _server(loop, "thefoxRpcServer")
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
	_server.setConnectionCallback(onRpcServerConnection);
	_server.setMessageCallback(onRpcServerMessage);
	_server.setCloseCallback(onRpcServerClose);

	return _server.start(listenAddr);
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
			
			gpb::Message *response(service->GetResponsePrototype(method).New());
            
			service->CallMethod(method, NULL, request, response, gpb::NewCallback(&gpb::DoNothing));
            
            //done
            rpc::Box box;
			rpc::Reply * reply = box.add_reply();
            reply->set_id(call->id());
            reply->set_result(true);
            reply->set_response(response->SerializeAsString());

			_server.send(conn,RpcCodec::encode(box));
			
			delete request;
			delete response;
		}
	}
}
