/*
 * @filename acceptor.h
 * @brief tcp acceptor
 * @author macwe1024 at gmail dot com
 */

#ifndef _THEFOX_NET_ACCEPTOR_H_
#define _THEFOX_NET_ACCEPTOR_H_

#include <base/common.h>
#include <net/callbacks.h>
#include <net/socket.h>
#include <net/event.h>
#include <net/inet_address.h>

namespace thefox
{

class Thread;

typedef std::function<void(SOCKET, const InetAddress &, const InetAddress &)> NewConnectionCallback;

class Acceptor
{
public:
    Acceptor(const InetAddress& listenAddr);
    ~Acceptor(void);

	bool init();
	bool listen();
	bool listening() const { return _listening; }
    void setNewConnectionCallback(const NewConnectionCallback &cb)
    { _newConnectionCallback = cb; }

private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(Acceptor);

	const InetAddress _listenAddr;
	Socket _acceptSocket;
	bool _listening;
    NewConnectionCallback _newConnectionCallback;

	void acceptLoop();
	Thread *_acceptThread;
#ifdef WIN32
	HANDLE _hAcceptEvent;
#endif
};

} // namespace thefox

#endif // _THEFOX_NET_ACCEPTOR_H_
