#include <net/rpc/RpcChannel.h>
#include <net/TcpConnection.h>
#include <net/rpc/RpcCodec.h>
#include <net/rpc/RpcServiceManager.h>
#include <net/rpc/RpcClient.h>

using namespace thefox;

RpcChannel::RpcChannel(RpcClient *rpcClient, const InetAddress &serverAddr)
	: _client(new TcpClient(rpcClient->eventloop(), "thefox.rpcchannel"))
{
	_client->setConnectionCallback(std::bind(&RpcChannel::onConnection, this, _1));
	_client->setMessageCallback(std::bind(&RpcChannel::onMessage, this, _1, _2, _3));
	_client->setCloseCallback(std::bind(&RpcChannel::onClose, this, _1));
	_client->open(serverAddr);
}

RpcChannel::~RpcChannel()
{
	_client->close();
}

void RpcChannel::CallMethod(const ::google::protobuf::MethodDescriptor* method,
                            ::google::protobuf::RpcController* controller,
                            const ::google::protobuf::Message* request,
                            ::google::protobuf::Message* response,
                            ::google::protobuf::Closure* done)
{
	rpc::Call *call = new rpc::Call();

	int64_t id = _id.inc();
	call->set_id(id);
	call->set_service(method->service()->name());
	call->set_method(method->name());
	call->set_request(request->SerializeAsString());

	RequestWaitPtr reqWait(new RequestWait(response, done));
	
	{
	MutexLockGuard lock(_mutex);
	_requests[id] = reqWait;
	}

	rpc::Box box;
	box.set_allocated_call(call);
	_conn->send(RpcCodec::encode(box));
	
	if (NULL == done) // synchronize
		reqWait->doneEvent.wait();
}

void RpcChannel::onConnection(const TcpConnectionPtr &conn)
{
	_conn = conn;
}

void RpcChannel::onClose(const TcpConnectionPtr &conn)
{}

void RpcChannel::onMessage(const TcpConnectionPtr &conn, Buffer *buf, const Timestamp recvTime)
{
	while (RpcCodec::isValid(buf->peek(), buf->readableBytes())) {
		rpc::Box box;
		size_t bufLen = buf->readableBytes();
		if (RpcCodec::parseFromArray(buf->peek(), bufLen, &box)) {
			if (box.has_reply())
				handleReplyMessage(box.reply(), Timestamp(Timestamp::now()));
			buf->retrieve(bufLen);
		}
	}
}

void RpcChannel::handleReplyMessage(const rpc::Reply &reply, Timestamp recvTime)
{
	const int64_t id = reply.id();

	RequestWaitPtr reqWait;

	{
	MutexLockGuard lock(_mutex);
	RequestWaitMap::iterator it = _requests.find(id);
	if (it != _requests.end()) {
		reqWait = it->second;
		_requests.erase(it);
	}
	}

	if (reqWait) {
		reqWait->response->ParseFromString(reply.response());
		reqWait->doneEvent.set();
		if (reqWait->done)
			reqWait->done->Run();
	}
}