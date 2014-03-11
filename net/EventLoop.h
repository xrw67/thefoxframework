#ifndef _THEFOX_NET_EVENTLOOP_H_
#define _THEFOX_NET_EVENTLOOP_H_

namespace thefox
{
namespace net
{

class Iocp;

class EventLoop
{
public:
	EventLoop(Iocp *iocp);
	~EventLoop();

	void loop();
private:
	Iocp *_iocp;
    bool _quit;
};

} // namespace net
} // namespace thefox

#endif // _THEFOX_NET_EVENTLOOP_H_