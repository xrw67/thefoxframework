#ifndef _THEFOX_NET_ACCEPTOR_H_
#define _THEFOX_NET_ACCEPTOR_H_

#include <boost/function.hpp>
#include <base/scoped_ptr.h>
#include <net/winapi.h>
#include <net/Socket.h>

namespace thefox
{

class Acceptor
{
public:
	static const int kMaxPostAccept = 10;

	typedef boost::function<void (SOCKET socket, const InetAddress &localAddr, const InetAddress &peerAddr)> NewConnectionCallback;
	
	Acceptor(const InetAddress &listenAddr);
	~Acceptor();
	
	SOCKET getSocketHandle () { return _acceptSocket.getSocketHandle(); }

	void setNewConnectionCallback(const NewConnectionCallback &cb)
	 { _newConnectionCallback = cb; }
	
	void listen();
	bool listening() const { return _listening; }
	
	void handleAccept(IoBuffer *ioBuffer);
private:
	typedef std::list<IoBuffer *> AcceptIoBufferList;

	bool _listening;
	Socket _acceptSocket;
	MutexLock _bufferLock;
	AcceptIoBufferList _acceptIoBuffers;
	NewConnectionCallback _newConnectionCallback;
};

}

#endif // _THEFOX_NET_ACCEPTOR_H_