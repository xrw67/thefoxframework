#include <net/rpc/RpcClient.h>
#include <net/rpc/RpcChannel.h>
#include <net/TcpConnection.h>

using namespace thefox;

RpcClient::RpcClient(EventLoop *loop)
	: _loop(loop)
{}

RpcClient::~RpcClient() 
{}
