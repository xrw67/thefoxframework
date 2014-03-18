#ifndef _THEFOX_NET_NETMSGQUEUE_H_
#define _THEFOX_NET_NETMSGQUEUE_H_

#include <list>
#include <base/Types.h>
#include <base/MutexLock.h>

namespace thefox
{

class NetMsg;

typedef void (*ProtobufMessageCallback)(NetMsg *msg);


class NetMsgQueue
{
public:
	explicit NetMsgQueue(const ProtobufMessageCallback& defaultCallback);
	~NetMsgQueue();
	
	void put(NetMsg *msg);
	NetMsg *pop();
	void startloop();
	void loop();
	void registerMessageCallback(const gpb::Descriptor *desc,
								const ProtobufMessageCallback &cb);
private:
	std::list<NetMsg *> m_msgs;
	MutexLock _lock;
	HANDLE _hMsgEvent;
	HANDLE _hQuitEvent;
	class NetMsgDispatcher;
	SharedPtr<NetMsgDispatcher> _dispatcher;
};

// 消息解析类宏定义

#define DECLARE_NET_MESSAGE_MAP \
	public: \
		static void Init(NetMsgQueue &); \
		static void onDefaultMessage(NetMsg *msg); \

#define BEGIN_NET_MESSAGE_MAP(theClass) \
	void theClass::onDefaultMessage(NetMsg *msg) \
	{ return; } \
	void theClass::Init(NetMsgQueue &queue) \
	{

#define END_NET_MESSAGE_MAP \
	}

#define ON_NET_MESSAGE(message, callback) \
	queue.RegisterMessageCallback(message::descriptor(), callback);

#define NET_MSG static

#define InitNetMessageMap(messageQueueClass) MessageParser::Init(messageQueueClass)

} // namespace thefox

#endif // _THEFOX_NET_NETMSGQUEUE_H_