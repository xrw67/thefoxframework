#ifndef _THEFOX_NET_CHANNEL_H_
#define _THEFOX_NET_CHANNEL_H_

namespace thefox
{

class Eventloop;

class Channel : noncopyable
{
public:
	typedef std::function<void()> EventCallback;
	typedef std::function<void(Timestamp)> ReadEventCallback;
	
	Channel(Eventloop *loop, HANDLE completePort);
};

}

#endif // _THEFOX_NET_CHANNEL_H_