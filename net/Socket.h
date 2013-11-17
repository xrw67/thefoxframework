#ifndef _THEFOX_NET_COSKET_H_
#define _THEFOX_NET_COSKET_H_

#include <base/noncopyable.h>
#include <net/inc.h>
#include <net/IoCompletionPort.h>
#include <net/InetAddress.h>
#include <net/IoContext.h>

namespace thefox
{

class Socket : noncopyable
{
public:
	Socket(const SOCKET socket)
		: _socket(socket)
	~Socket()
	{}
	void create();
	bool bind(const InetAddress &listenAddr);
	bool listen();
	bool connect(const InetAddress &addr);

	bool postAccpt(IoContext *acceptIoContext);
	bool postSend();
	bool postRecv();
	void close();
private:
	const SOCKET _socket;
};

}

#endif //_THEFOX_NET_COSKET_H_