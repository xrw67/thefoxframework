#ifndef _THEFOX_NET_ACCEPTOR_H_
#define _THEFOX_NET_ACCEPTOR_H_

#include <base/noncopyable.h>
#include <base/scoped_ptr.h>
#include <net/winapi.h>

namespace thefox
{

class Socket;
class IoBuffer;
class IoCompletionPort;

class Acceptor : noncopyable
{
public:
	static const int kMaxPostAccept = 10;

	typedef std::function<void (SOCKET socket, const InetAddress&)> NewConnectionCallback;
	
	Acceptor(IoCompletionPort * const iocpPtr, const InetAddress &listenAddr);
	~Acceptor();
	
	void setNewConnectionCallback(const NewConnectionCallback &cb)
	 { _newConnectionCallback = cb; }
	
	void listen();
	bool listening() const { return _listening; }
	
private:
	void handleAccept();

	bool _listening;
	IoCompletionPort * const _iocpPtr;
	scoped_ptr<Socket> _acceptSocket;
	std::vector<scoped_ptr<IoBuffer>> _acceptIoContexts;
	NewConnectionCallback _newConnectionCallback;
};

}

#endif // _THEFOX_NET_ACCEPTOR_H_