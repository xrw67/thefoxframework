#include <net/rpc/RpcClient.h>
#include <net/rpc/RpcChannel.h>
#include <net/TcpConnection.h>

using namespace thefox;

void defaultNonRpcMsg(const std::string &msgType, const gpb::Message *message)
{
	
}

RpcClient::RpcClient(EventLoop *loop)
	: _loop(loop)
	, _nonRpcMsgCallback(defaultNonRpcMsg)
{}

RpcClient::~RpcClient() 
{}
