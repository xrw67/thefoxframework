#include <net/acceptor.h>
#include <log/logging.h>
#include <net/event_loop.h>
#include <net/inet_address.h>
#include <net/tcp_server.h>

#ifdef WIN32
#include <base/thread.h>
#endif

using namespace thefox;

Acceptor::Acceptor(TcpServer *server,const InetAddress& listenAddr)
	: _server(server)
	, _listening(false)
{
	_acceptSocket.create();
	_acceptSocket.bind(listenAddr);
#ifdef WIN32
	_acceptThread = new Thread(std::bind(&Acceptor::acceptLoop, this), "acceptor.acceptloop()");
#endif
}

Acceptor::~Acceptor()
{
	_listening = false;
	//remove ioevent
#ifdef WIN32
	::CloseHandle(_hAcceptEvent);
	_acceptThread->stop();
	delete _acceptThread;
#else

#endif

}

bool Acceptor::listen()
{
	if (_listening)
		return true;

	if (_acceptSocket.listen()) {
		_listening = true;

		// set ioevent
#ifdef WIN32
		_hAcceptEvent = ::WSACreateEvent();
		WSAEventSelect(_acceptSocket.fd(), _hAcceptEvent, FD_ACCEPT);
		_acceptThread->start();
#else

#endif
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

					_server->handleNewConnection(clientSockfd, peerAddr);
                }
            }
        }
    }
}
#else
void Acceptor::handleAccept(IoEvent *ev)
{
	InetAddress peerAddr;
	SOCKET clientSockfd = _acceptSocket.accept(&peerAddr);
	if (INVALID_SOCKET == clientSockfd)
		THEFOX_LOG(ERROR) << "accept a socket error";

	_server->handleNewConnection(clientSockfd, peerAddr);
}
#endif