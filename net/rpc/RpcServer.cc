#include <net/rpc/RpcServer.h>
#include <net/Buffer.h>
#include <net/Connection.h>
#include <net/rpc/RpcCodec.h>
#include <net/rpc/RpcChannel.h>
#include <net/rpc/RpcServiceManager.h>
#include <net/rpc/RpcService.h>
#include <net/rpc/TaskManager.h>

using namespace thefox;

RpcServer::RpcServer(EventLoop *loop)
	: _server(new TcpServer(loop, "thefoxRpcServer"))
	, _serviceManager(new RpcServiceManager())
	, _rpcServiceImpl(new RpcServiceImpl(_serviceManager))
{
	registerService(_rpcServiceImpl.get());
	_taskManager = std::make_shared<TaskManager>(
		std::bind(&RpcServer::handleCallMessage, this, _1, _2, _3), NULL);
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

void RpcServer::sendOneway(const ConnectionPtr &conn, const gpb::Message *message)
{
	const std::string& typeName = message->GetTypeName();
	rpc::OnewayMessage *oneway = new rpc::OnewayMessage();
    oneway->set_type(typeName);
	oneway->set_body(message->SerializeAsString());

	rpc::Box box;
	box.set_allocated_oneway(oneway);
	_server->send(conn, RpcCodec::encode(box));
}

void RpcServer::onConnection(const ConnectionPtr &conn)
{}

void RpcServer::onClose(const ConnectionPtr &conn)
{}

void RpcServer::onMessage(const ConnectionPtr &conn, Buffer *buf, const Timestamp recvTime)
{
	while (RpcCodec::isValid(buf->peek(), buf->readableBytes())) {
		BoxPtr box(new rpc::Box());
		size_t bufLen = buf->readableBytes();
		if (RpcCodec::parseFromArray(buf->peek(), bufLen, box)) {
			_taskManager->pushBox(conn, recvTime, box);
			buf->retrieve(bufLen);
		}
	}
}

void RpcServer::handleCallMessage(const ConnectionPtr &conn, const rpc::Call &call, Timestamp receiveTime)
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
            reply->set_result(rpc::Reply_Result_kOk);
            reply->set_response(response->SerializeAsString());

			rpc::Box box;
			box.set_allocated_reply(reply);
			_server->send(conn, RpcCodec::encode(box));
			
			delete request;
			delete response;
		}
	}
}