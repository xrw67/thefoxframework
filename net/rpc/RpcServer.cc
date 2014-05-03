#include <net/rpc/RpcServer.h>
#include <net/Buffer.h>
#include <net/TcpConnection.h>
#include <net/rpc/RpcCodec.h>
#include <net/rpc/RpcChannel.h>
#include <net/rpc/RpcServiceManager.h>
#include <net/rpc/RpcService.h>

using namespace thefox;

RpcServer::RpcServer(EventLoop *loop)
	: _server(new TcpServer(loop, "thefoxRpcServer"))
	, _serviceManager(new RpcServiceManager())
	, _rpcServiceImpl(new RpcServiceImpl(_serviceManager))
{
	registerService(_rpcServiceImpl.get());
}

RpcServer::~RpcServer() 
{}

void RpcServer::registerService(gpb::Service *service)
{
	_serviceManager->registerService(service);
}
void RpcServer::setHearthBeathCallback(const HeartBeathCallback &cb)
{ 
	_rpcServiceImpl->setHearthBeathCallback(cb); 
}

bool RpcServer::start(const InetAddress &listenAddr)
{
	_server->setConnectionCallback(std::bind(&RpcServer::onConnection, this, _1));
	_server->setMessageCallback(std::bind(&RpcServer::onMessage, this, _1, _2, _3));
	_server->setCloseCallback(std::bind(&RpcServer::onClose, this, _1));

	return _server->start(listenAddr);
}

void RpcServer::sendNonRpcMsg(const TcpConnectionPtr &conn, const gpb::Message *message)
{
	const std::string& typeName = message->GetTypeName();
	rpc::NonRpcMsg *nrm = new rpc::NonRpcMsg();
    nrm->set_msg_type(typeName);
	nrm->set_msg_body(message->SerializeAsString());

	rpc::Box box;
	box.set_allocated_nonrpc_msg(nrm);
	_server->send(conn, RpcCodec::encode(box));
}

void RpcServer::onConnection(const TcpConnectionPtr &conn)
{}

void RpcServer::onClose(const TcpConnectionPtr &conn)
{}

void RpcServer::onMessage(const TcpConnectionPtr &conn, Buffer *buf, const Timestamp recvTime)
{
	while (RpcCodec::isValid(buf->peek(), buf->readableBytes())) {
		rpc::Box box;
		size_t bufLen = buf->readableBytes();
		if (RpcCodec::parseFromArray(buf->peek(), bufLen, &box)) {
			if (box.has_call())
				handleCallMessage(conn, box.call(), recvTime);
			buf->retrieve(bufLen);
		}
	}
}

void RpcServer::handleCallMessage(const TcpConnectionPtr &conn, const rpc::Call &call, Timestamp receiveTime)
{
	if (NULL == _serviceManager)
		return;

	gpb::Service *service = _serviceManager->findService(call.service());
	if (NULL != service) {
		const gpb::ServiceDescriptor *desc = service->GetDescriptor();
		const gpb::MethodDescriptor *method = desc->FindMethodByName(call.method());
		
		if (method) {
			gpb::Message *request(service->GetRequestPrototype(method).New());
			request->ParseFromString(call.request());
			
			gpb::Message *response(service->GetResponsePrototype(method).New());
            
			service->CallMethod(method, NULL, request, response, gpb::NewCallback(&gpb::DoNothing));
            
            //done
           
			rpc::Reply *reply = new rpc::Reply();
            reply->set_id(call.id());
            reply->set_result(true);
            reply->set_response(response->SerializeAsString());

			rpc::Box box;
			box.set_allocated_reply(reply);
			_server->send(conn, RpcCodec::encode(box));
			
			delete request;
			delete response;
		}
	}
}