#ifndef _THEFOX_NET_MSGQUEUE_MESSAGE_H_
#define _THEFOX_NET_MSGQUEUE_MESSAGE_H_

#include <base/Types.h>
#include <base/Timestamp.h>
#include <net/Callbacks.h>
#include <net/rpc/common.h>
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>

namespace thefox
{

/// @brief 表示消息队列中的一个消息
class MsgBox
{
public:
	explicit MsgBox(const TcpConnectionPtr &sender, gpb::Message *msg)
		: _time(Timestamp::now())
		, _sender(sender)
	{
		_msg.reset(msg);
	}
	~MsgBox()
	{}

	const Timestamp &time() const { return _time; }
	TcpConnectionPtr &sender() { return _sender; }
	const gpb::Message *message() { return _msg.get(); }
	const gpb::Descriptor *descriptor() { return _msg->GetDescriptor(); }

private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(MsgBox);
	MsgBox();

	const Timestamp _time;
	TcpConnectionPtr _sender;
	std::unique_ptr<gpb::Message> _msg;
};

typedef std::shared_ptr<MsgBox> MsgBoxPtr;

} // namespace thefox

#endif // _THEFOX_NET_MSGQUEUE_MESSAGE_H_
