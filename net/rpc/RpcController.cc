#include <net/rpc/RpcController.h>
#include <google/protobuf/message.h>

using namespace thefox;

RpcController::RpcController()
	: _timeout(30 *1000)
{
}

RpcController::~RpcController()
{
}