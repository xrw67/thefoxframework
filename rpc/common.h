#ifndef _THEFOX_RPC_COMMON_H_
#define _THEFOX_RPC_COMMON_H_

#include <net/callbacks.h>
#include <rpc/rpc.pb.h>

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

typedef std::function<void(TcpConnection *sender, 
							const std::string &type, 
							const gpb::Message *message,
							const Timestamp &receiveTime)> OnewayCallback;

typedef std::function<void()> HeartBeathCallback;
typedef std::function<void(TcpConnection *conn, 
						const rpc::Reply reply, 
						Timestamp recvTime)> ReplyCallback;
typedef std::function<void(TcpConnection *conn, 
						const rpc::Call call, 
						Timestamp recvTime)> CallCallback;

} //namespace thefox;

#endif // _THEFOX_RPC_COMMON_H_