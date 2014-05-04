#include <net/rpc/RpcChannel.h>
#include <net/TcpConnection.h>
#include <net/rpc/RpcServiceManager.h>
#include <net/rpc/RpcClient.h>
#include <net/rpc/RpcCodec.h>
#include <net/rpc/MqManager.h>

using namespace thefox;

RpcChannel::RpcChannel(RpcClient *rpcClient, const InetAddress &serverAddr)
	: _client(new TcpClient(rpcClient->eventloop(), "thefox.rpcchannel"))
	, _rpcClient(rpcClient)
{
	rpcClient->registerChannel(this);

	_client->setConnectionCallback(std::bind(&RpcChannel::onConnection, this, _1));
	_client->setMessageCallback(std::bind(&RpcChannel::onMessage, this, _1, _2, _3));
	_client->setCloseCallback(std::bind(&RpcChannel::onClose, this, _1));
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
	_rpcClient->CallMethod(_conn, method, controller, request, response, done);
}

void RpcChannel::setMqManager(const MqManagerPtr &mqManager)
{
	_mqManager = mqManager; 
}

void RpcChannel::onConnection(const TcpConnectionPtr &conn)
{
	_conn = conn;
}

void RpcChannel::onClose(const TcpConnectionPtr &conn)
{}

void RpcChannel::onMessage(const TcpConnectionPtr &conn, Buffer *buf, const Timestamp &recvTime)
{
	while (RpcCodec::isValid(buf->peek(), buf->readableBytes())) {
		BoxPtr box(new rpc::Box());
		size_t bufLen = buf->readableBytes();
		if (RpcCodec::parseFromArray(buf->peek(), bufLen, box)) {
			_mqManager->pushBox(conn, recvTime, box);
			buf->retrieve(bufLen);
		}
	}
}

