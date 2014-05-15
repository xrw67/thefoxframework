#include <net/rpc/rpc_controller.h>
#include <google/protobuf/message.h>

using namespace thefox;

RpcController::RpcController()
	: _timeout(30*1000)
{
}

RpcController::~RpcController()
{
}