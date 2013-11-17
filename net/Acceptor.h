#ifndef _THEFOX_NET_ACCEPTOR_H_
#define _THEFOX_NET_ACCEPTOR_H_

#include <base/noncopyable.h>
#include <base/MutexLock.h>
#include <net/inc.h>

class Socket;
class IoContext;

namespace thefox
{

class Acceptor : noncopyable
{
public:
	static const int kMaxPostAccept = 10;

	typedef std::function<void (SOCKET socket, const InetAddress&)> NewConnectionCallback;
	Acceptor(Socket *socket, const InetAddress &listenAddr);
	~Acceptor();
	
	void setNewConnectionCallback(const NewConnectionCallback &cb)
	 { _newConnectionCallback = cb; }
	
	void listen();
	bool listening() const { return _listening; }
	
private:
	void handleAccept();
	bool postAccept(IoContext *acceptIoContext);
	
	MutexLock _lock;
	bool _listening;
	std::unique_ptr<Socket> _acceptSocket;
	LPFN_ACCEPTEX                _lpfnAcceptEx;
	LPFN_GETACCEPTEXSOCKADDRS    _lpfnGetAcceptExSockAddrs; 
	NewConnectionCallback _newConnectionCallback;
	std::vector<IoContext *> _acceptIoContexts;
};

}

#endif // _THEFOX_NET_ACCEPTOR_H_