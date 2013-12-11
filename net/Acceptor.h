#ifndef _THEFOX_NET_ACCEPTOR_H_
#define _THEFOX_NET_ACCEPTOR_H_

#include <base/scoped_ptr.h>
#include <net/winapi.h>

namespace thefox
{

class InetAddress;
class IoBuffer;

class Acceptor
{
public:
	Acceptor(const InetAddress &listenAddr);
	~Acceptor();
	
	void listen();
	void stop();
	void insertPengingIoBuffer(IoBuffer *buf) { _IoBuffers.push_back(buf); }
	bool postAccept(IoBuffer *buf);
	void getAcceptExSockAddrs(IoBuffer *buf, InetAddress &localAddr, InetAddress &peerAddr);

	bool _listening;
	SOCKET _acceptSocket;
	HANDLE _acceptEvent;
	HANDLE _stopEvent;

	MutexLock _lock;
	std::list<IoBuffer *> _IoBuffers;

	LPFN_ACCEPTEX _lpfnAcceptEx;
	LPFN_GETACCEPTEXSOCKADDRS _lpfnGetAcceptExSockAddrs; 
};

}

#endif // _THEFOX_NET_ACCEPTOR_H_