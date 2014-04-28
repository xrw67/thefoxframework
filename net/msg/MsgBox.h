#ifndef _THEFOX_NET_MSG_MSGBOX_H_
#define _THEFOX_NET_MSG_MSGBOX_H_

#include <base/Types.h>
#include <net/Callbacks.h>
#include <net/msg/MsgCodec.h>

namespace thefox
{

class MsgBox
{
public:
	MsgBox() {}
	~MsgBox() {}
	
	void setConnection(const TcpConnectionPtr &conn)
	{ _conn = conn; }
	
	void setBox(msg::Box *box)
	{ 
		_box.reset(box); 
		_msgBody.reset(MsgCodec::getMessageFromBox(box));
	}

	const gpb::Descriptor *descriptor() const
	{ return _msgBody->GetDescriptor(); }

private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(MsgBox);
	std::unique_ptr<msg::Box> _box;
	std::unique_ptr<gpb::Message> _msgBody;
	TcpConnectionPtr _conn;
};

typedef std::shared_ptr<MsgBox> MsgBoxPtr;

} // namespace thefox

#endif // _THEFOX_NET_MSG_MSGBOX_H_