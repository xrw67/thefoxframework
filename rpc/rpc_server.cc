#include <rpc/rpc_server.h>
#include <net/buffer.h>
#include <net/tcp_connection.h>
#include <rpc/rpc_codec.h>
#include <rpc/rpc_channel.h>
#include <rpc/rpc_service_manager.h>
#include <rpc/rpc_service.h>
#include <rpc/task_manager.h>

using namespace thefox;

RpcServer::RpcServer(EventLoop *loop, const InetAddress &listenAddr)
	: _server(new TcpServer(loop, listenAddr, "thefoxRpcServer"))
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

bool RpcServer::start()
{
	_server->setConnectionCallback(std::bind(&RpcServer::onConnection, this, _1));
	_server->setMessageCallback(std::bind(&RpcServer::onMessage, this, _1, _2, _3));

	return _server->start();
}

void RpcServer::sendOneway(TcpConnection *conn, const gpb::Message *message)
{
	const std::string& typeName = message->GetTypeName();
	rpc::OnewayMessage *oneway = new rpc::OnewayMessage();
    oneway->set_type(typeName);
	oneway->set_body(message->SerializeAsString());

	rpc::Box box;
	box.set_allocated_oneway(oneway);
	conn->send(RpcCodec::encode(box));
}

void RpcServer::onConnection(TcpConnection *conn)
{}

void RpcServer::onMessage(TcpConnection *conn, Buffer *buf, const Timestamp recvTime)
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

void RpcServer::handleCallMessage(TcpConnection *conn, const rpc::Call &call, Timestamp receiveTime)
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
			conn->send(RpcCodec::encode(box));
			
			delete request;
			delete response;
		}
	}
}