#ifndef _THEFOX_NET_RPC_TASK_H_
#define _THEFOX_NET_RPC_TASK_H_

#include <base/Timestamp.h>
#include <net/Callbacks.h>
#include <net/rpc/common.h>
#include <net/rpc/rpc.pb.h>

namespace thefox
{

/// @brief 表示一个消息
class Task
{
public:
	explicit Task(const TcpConnectionPtr &sender, 
					const Timestamp &receiveTime, 
					const BoxPtr &msg)
		: _sender(sender) 
		, _time(receiveTime) 
		, _box(msg) 
	{}

	const Timestamp &time() const { return _time; }
	TcpConnectionPtr &sender() { return _sender; }
	bool hasCall() const { return _box->has_call(); }
	bool hasReply() const { return _box->has_reply(); }
	bool hasOob() const { return _box->has_oob(); }
	const rpc::Call &call() const { return _box->call(); }
	const rpc::Reply &reply() const { return _box->reply(); }
	const rpc::OutOfBand &oob() const { return _box->oob(); }

private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(Task);
	const Timestamp _time;
	TcpConnectionPtr _sender;
	const BoxPtr _box;
};

typedef std::shared_ptr<Task> TaskPtr;

} // namespace thefox

#endif // _THEFOX_NET_RPC_TASK_H_
