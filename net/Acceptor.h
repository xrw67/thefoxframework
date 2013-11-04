#ifndef _THEFOX_NET_ACCEPTOR_H
#define _THEFOX_NET_ACCEPTOR_H

#include <functionl>
#include <base/noncopyable.hpp>
#include <net/Channel.h>
#include <net/Socket.h>

namespace thefox
{

class EventLoop;
class InetAddress;

///
/// Acceptor of incoming TCP connections.
///
class Acceptor : noncopyable
{
public:
	typedef std::function<void (int sockfd,const InetAddress&)> NewConnectionCallback;

	Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reuseport);
	~Acceptor();

	void setNewConnectionCallback(const NewConnectionCallback& cb) { _newConnectionCallback = cb; }

	bool listenning() const { return _listenning; }
	void listen();

private:
	void handleRead();

	EventLoop* _loop;
	Socket _acceptSocket;
	Channel _acceptChannel;
	NewConnectionCallback _newConnectionCallback;
	bool _listenning;
	int _idleFd;
};

}

#endif  // _THEFOX_NET_ACCEPTOR_H
