#ifndef _THEFOX_NET_COSKET_H_
#define _THEFOX_NET_COSKET_H_

#include <base/noncopyable.h>
#include <net/inc.h>
#include <net/IoCompletionPort.h>
#include <net/InetAddress.h>

class IoContext;
class IoCompletionPort;

namespace thefox
{

class Socket : noncopyable
{
public:
	Socket(IoCompletionPort *iocp);
	~Socket();
	bool create();
	bool addToIocp(ULONG_PTR completionKey);
	void listen()
	{ ::listen(_socket, SOMAXCONN); }
	bool bind(const InetAddress &listenAddr);
	bool postAccpt(IoContext *acceptIoContext);
	bool postSend();
	bool postRecv();
	void close();
private:
	SOCKET _socket;
	IoCompletionPort *_iocp;
};

}

#endif //_THEFOX_NET_COSKET_H_