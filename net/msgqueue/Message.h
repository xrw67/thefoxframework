#ifndef _THEFOX_NET_MSGQUEUE_MESSAGE_H_
#define _THEFOX_NET_MSGQUEUE_MESSAGE_H_

#include <base/Types.h>
#include <base/Timestamp.h>


namespace thefox
{

namespace mq
{

class Message 
{
public:
	Message(const std::string &msgType, const std::string *msgBody = NULL)
	: _time(Timestamp::now())
	, _msgType(msgType)
	, _data(msgBody)
	 {}
	~Message() 
	{ SAFEDELETE(data); }

	const Timestamp _time;
	const std::string _msgType;
	std::string *_msgBody;
};

typedef std::shared_ptr<Message> MessagePtr;

} // namespace mq

} // namespace thefox

#endif // _THEFOX_NET_MSGQUEUE_MESSAGE_H_
