#include <net/netmsg/NetMsgQueue.h>
#include <net/netmsg/NetMsg.h>

namespace thefox
{
	
class NetMsgDispatcher
{
public:
	explicit NetMsgDispatcher(const ProtobufMessageCallback& defaultCallback)
		: _defaultCallback(defaultCallback)
	{}

	~NetMsgDispatcher(void)
	{}

	void onMessage(NetMsg *msg) const
	{
		CallbackMap::const_iterator it = m_callbacks.find(msg->getDescriptor());
		if (it != m_callbacks.end())
			it->second(msg);
		else
			m_defaultCallback(msg);
	}

	void registerMessageCallback(const gpb::Descriptor *desc, const ProtobufMessageCallback &cb)
	{ m_callbacks[desc] = cb; }

private:
	typedef std::map<const gpb::Descriptor *, ProtobufMessageCallback> CallbackMap;
	
	CallbackMap _callbacks;
	ProtobufMessageCallback _defaultCallback;
};
	
DWORD WINAPI parseMsgThreadProc(LPVOID param)
{
	NetMsgQueue *queue = reinterpret_cast<NetMsgQueue *>(param);
	queue->loop();
	return 0;
}

} // namespace thefox

using namespace thefox;

NetMsgQueue::NetMsgQueue(const NmsMessageCallback& defaultCb)
	: _dispatcher(new NetMsgDispatcher(defaultCb))
{
	_hMsgEvent = CreateEvent(NULL, TRUE, FALSE, NULL)
	_hQuitEvnet = CreateEvent(NULL, TRUE, FALSE, NULL)
}

NetMsgQueue::~NetMsgQueue()
{
	CloseHandle(_hMsgEvent);
	CloseHandle(_hQuitEvnet);
}

void NetMsgQueue::put(NetMsg *msg) 
{ 
	MutexLockGuard lock(_lock);
	m_msgs.push_back(msg); 
	SetEvent(_hMsgEvent);
}

NetMsg *NetMsgQueue::pop()
{
	MutexLockGuard lock(_lock);
	if (m_msgs.empty()) {
		ResetEvent(_hMsgEvent);
		return NULL
	}
	NetMsg *msg = m_msgs.front();
	m_msgs.pop_front();
	return msg;
}

void NetMsgQueue::registerMessageCallback(const google::protobuf::Descriptor *desc, 
							 const NmsMessageCallback &cb)
{
	_dispatcher->registerMessageCallback(desc, cb);
}

void NetMsgQueue::startloop()
{
	HANDLE handle = CreateThread(NULL, 0, parseMsgThreadProc, this, 0, NULL);
	CloseHandle(handle);
}

void NetMsgQueue::loop()
{
	while (WAIT_OBJECT_0 != WaitForSingleObject(_hQuitEvent, 0)) {
		CMsg *msg = NULL;
		while (WAIT_OBJECT_0 == WaitForSingleObject(_hMsgEvent, INFINITE)) {

		MutexLockGuard lock(_lock);
		if (!m_msgs.empty())
			msg = pop();
		else
			Sleep(100);
			continue;
		}
		if (msg)
		{
			m_dispatcher.onMessage(msg);
			delete msg;
		}
	}
}
