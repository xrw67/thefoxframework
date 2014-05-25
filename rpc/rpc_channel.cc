#include <rpc/rpc_channel.h>
#include <net/tcp_connection.h>
#include <rpc/rpc_service_manager.h>
#include <rpc/rpc_client.h>
#include <rpc/rpc_codec.h>
#include <rpc/task_manager.h>
#include <rpc/rpc_controller.h>

using namespace thefox;

RpcChannel::RpcChannel(RpcClient *rpcClient, const InetAddress &serverAddr)
	: _client(new TcpClient(rpcClient->eventloop(), "thefox.rpcchannel"))
	, _rpcClient(rpcClient)
{
	rpcClient->registerChannel(this);

	_client->setConnectionCallback(std::bind(&RpcChannel::onConnection, this, _1));
	_client->setMessageCallback(std::bind(&RpcChannel::onMessage, this, _1, _2, _3));
	_client->open(serverAddr);
}

RpcChannel::~RpcChannel()
{
	_client->close();
	_rpcClient->unregisterChannel(this);
}

void RpcChannel::CallMethod(const ::google::protobuf::MethodDescriptor* method,
                            ::google::protobuf::RpcController* controller,
                            const ::google::protobuf::Message* request,
                            ::google::protobuf::Message* response,
                            ::google::protobuf::Closure* done)
{
	_rpcClient->CallMethod(_conn, method, dynamic_cast<RpcController *>(controller), request, response, done);
}

void RpcChannel::setTaskManager(const TaskManagerPtr &taskManager)
{
	_taskManager = taskManager; 
}

void RpcChannel::onConnection(TcpConnection *conn)
{
	_conn = conn;
}

void RpcChannel::onMessage(TcpConnection *conn, Buffer *buf, Timestamp recvTime)
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

