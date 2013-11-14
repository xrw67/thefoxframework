#ifndef _THEFOX_NET_ACCEPTOR_H_
#define _THEFOX_NET_ACCEPTOR_H_

namespace thefox
{

class Acceptor : noncopyable
{
public:
	typedef boost::function<void (SOCKET socket, const InetAddress&)> NewConnectionCallback;
	Acceptor(const InetAddress &listenAddr);
	~Acceptor();
	
	void setNewConnectionCallback(const NewConnectionCallback &cb)
	 { _newConnectionCallback = cb; }
	
	void listen();
	bool listening() const { return _listening; }
	
private:
	LPFN_ACCEPTEX                _lpfnAcceptEx;
	LPFN_GETACCEPTEXSOCKADDRS    _lpfnGetAcceptExSockAddrs; 
	SOCKET _socket;
	NewConnectionCallback _newConnectionCallback;
	bool _listening;
};

}

#endif // _THEFOX_NET_ACCEPTOR_H_