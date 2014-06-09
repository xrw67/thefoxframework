#include <net/acceptor.h>
#include <log/logging.h>
#include <net/event_loop.h>
#include <net/tcp_server.h>
#include <base/thread.h>

using namespace thefox;

Acceptor::Acceptor(const InetAddress& listenAddr)
	: _listenAddr(listenAddr)
	, _listening(false)
	, _acceptSocket(Socket::create())
{
}

Acceptor::~Acceptor()
{
	_listening = false;
	_acceptThread->stop();
	delete _acceptThread;
#ifdef WIN32
	::CloseHandle(_hAcceptEvent);
#endif
}

bool Acceptor::init()
{
	THEFOX_TRACE_FUNCTION;

	assert(_acceptSocket.fd() >= 0);

	bool ret = _acceptSocket.bind(_listenAddr);
	
	_acceptThread = new Thread(std::bind(&Acceptor::acceptLoop, this), "acceptor.acceptloop()");
	
	return ret;
}

bool Acceptor::listen()
{
	THEFOX_TRACE_FUNCTION;

	if (_listening)
		return true;

	if (_acceptSocket.listen()) {
		_listening = true;

#ifdef WIN32
		_hAcceptEvent = ::WSACreateEvent();
		WSAEventSelect(_acceptSocket.fd(), _hAcceptEvent, FD_ACCEPT);
#endif
		_acceptThread->start();
		THEFOX_LOG(INFO) << "Acceptor::listen() done";
		return true;
	} else {
		THEFOX_LOG(ERROR) << "Acceptor::listen() failed";
		return false;
	}
}

#ifdef WIN32
void Acceptor::acceptLoop()
{
    while (_listening) {
        if (WSA_WAIT_TIMEOUT != ::WSAWaitForMultipleEvents(1,&_hAcceptEvent, FALSE, 100, FALSE)) {
            WSANETWORKEVENTS events;
            if (SOCKET_ERROR != ::WSAEnumNetworkEvents(_acceptSocket.fd(), _hAcceptEvent, &events)) {
                if (events.lNetworkEvents & FD_ACCEPT && 0 == events.iErrorCode[FD_ACCEPT_BIT]) {
                    InetAddress peerAddr;
					SOCKET clientSockfd = _acceptSocket.accept(&peerAddr);
					if (INVALID_SOCKET == clientSockfd)
						THEFOX_LOG(ERROR) << "accept a socket error";

                    if (_newConnectionCallback)
                        _newConnectionCallback(clientSockfd, _listenAddr, peerAddr);
                }
            }
        }
    }
}
#else
void Acceptor::acceptLoop()
{
	THEFOX_TRACE_FUNCTION;

	InetAddress peerAddr;
	SOCKET clientSockfd = _acceptSocket.accept(&peerAddr);
	if (INVALID_SOCKET == clientSockfd)
		THEFOX_LOG(ERROR) << "accept a socket error";

	_server->handleNewConnection(clientSockfd, _listenAddr, peerAddr);
}
#endif
