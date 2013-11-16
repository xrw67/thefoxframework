#ifndef _THEFOX_NET_ACCEPTOR_H_
#define _THEFOX_NET_ACCEPTOR_H_

class IoContext;

namespace thefox
{

class Acceptor : noncopyable
{
public:
	typedef boost::function<void (SOCKET socket, const InetAddress&)> NewConnectionCallback;
	Acceptor(const HANDLE completionPort, const InetAddress &listenAddr);
	~Acceptor();
	
	void setNewConnectionCallback(const NewConnectionCallback &cb)
	 { _newConnectionCallback = cb; }
	
	void listen();
	bool listening() const { return _listening; }
	
private:
	void handleAccept();
	bool postAccept(IoContext *acceptIoContext);
		
	static const int kMaxPostAccept = 10;
	
	const HANDLE _completionPort;
	LPFN_ACCEPTEX                _lpfnAcceptEx;
	LPFN_GETACCEPTEXSOCKADDRS    _lpfnGetAcceptExSockAddrs; 
	SOCKET _socket;
	NewConnectionCallback _newConnectionCallback;
	std::vector<IoContext *> _acceptIoContexts;
	MutexLock _lock;
	bool _listening;
};

}

#endif // _THEFOX_NET_ACCEPTOR_H_