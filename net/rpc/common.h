#ifndef _THEFOX_RPC_COMMON_H_
#define _THEFOX_RPC_COMMON_H_

#include <net/Callbacks.h>
#include <net/rpc/rpc.pb.h>

namespace google
{
namespace protobuf
{

typedef std::shared_ptr<Message> MessagePtr;

}
}

namespace thefox
{

#define gpb ::google::protobuf


class RpcServiceManager;
typedef std::shared_ptr<RpcServiceManager> ServiceManagerPtr;
typedef std::shared_ptr<rpc::Box> BoxPtr;

class TaskManager;
typedef std::shared_ptr<TaskManager> TaskManagerPtr;

typedef std::function<void(const TcpConnectionPtr &sender, 
							const std::string &type, 
							const gpb::Message *message,
							const Timestamp &receiveTime)> OnewayCallback;

typedef std::function<void()> HeartBeathCallback;
typedef std::function<void(const TcpConnectionPtr &conn, 
						const rpc::Reply reply, 
						const Timestamp &recvTime)> ReplyCallback;
typedef std::function<void(const TcpConnectionPtr &conn, 
						const rpc::Call call, 
						const Timestamp &recvTime)> CallCallback;

} //namespace thefox;

#endif // _THEFOX_RPC_COMMON_H_