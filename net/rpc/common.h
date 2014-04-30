#ifndef _THEFOX_RPC_COMMON_H_
#define _THEFOX_RPC_COMMON_H_

#include <google/protobuf/message.h>

namespace google
{
namespace protobuf
{
}
}

namespace thefox
{

#define gpb ::google::protobuf

typedef std::function<void(const std::string &msgType, const gpb::Message *message)> NonRpcMsgCallback;

} //namespace thefox;

#endif // _THEFOX_RPC_COMMON_H_