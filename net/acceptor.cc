#include <net/acceptor.h>
#include <log/logging.h>
#include <net/event_loop.h>
#ifndef WIN32
#include <sys/epoll.h>
#endif
using namespace thefox;
using namespace thefox::net;

Acceptor::Acceptor(EventLoop *loop, const InetAddress& listenAddr)
	: _loop(loop)
	, _listenAddr(listenAddr)
	, _listening(false)
	, _acceptSocket(Socket::create())
{
	Socket::setNonBlock(_acceptSocket.fd());
}

Acceptor::~Acceptor()
{
	_listening = false;

#ifdef WIN32
	::CloseHandle(_hAcceptEvent);
    _acceptThread->stop();
    delete _acceptThread;
#else
	_loop->delEvent(&_event);
#endif
}

bool Acceptor::init()
{
	THEFOX_TRACE_FUNCTION;

	assert(_acceptSocket.fd() >= 0);

    bool ret = _acceptSocket.bind(_listenAddr);

#ifdef WIN32
    _acceptThread = new Thread(std::bind(&Acceptor::acceptLoop, this));
#else
	_event.fd = _acceptSocket.fd();
#endif

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
		_acceptThread->start();
#else
		_loop->addEvent(&_event);
		_event.handler = std::bind(&Acceptor::onAccept, this, _1, _2);
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
        if (WSA_WAIT_TIMEOUT == ::WSAWaitForMultipleEvents(1, &_hAcceptEvent, FALSE, 100, FALSE)) {
        	continue;
        }

        WSANETWORKEVENTS events;
        if (SOCKET_ERROR == ::WSAEnumNetworkEvents(_acceptSocket.fd(), _hAcceptEvent, &events)) {
        	THEFOX_LOG(ERROR) << "WSAEnumNetworkEvents() error";
        	continue;
        }

        if (events.lNetworkEvents & FD_ACCEPT && 0 == events.iErrorCode[FD_ACCEPT_BIT]) {
            InetAddress peerAddr;
			SOCKET clientSockfd = _acceptSocket.accept(&peerAddr);
			if (INVALID_SOCKET == clientSockfd) {
				THEFOX_LOG(ERROR) << "accept a socket error";
			}

            if (_newConnectionCallback) {
                _newConnectionCallback(clientSockfd, _listenAddr, peerAddr);
            }
        }
    }
}
#else
void Acceptor::onAccept(Event *ev, void *arg)
{
	THEFOX_TRACE_FUNCTION;

	uint32_t revents = (uint32_t)arg;

	if (revents & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
		InetAddress peerAddr;
		SOCKET clientSockfd = _acceptSocket.accept(&peerAddr);
		if (INVALID_SOCKET == clientSockfd) {
			THEFOX_LOG(ERROR) << "accept a socket error";
		}
		
		if (_newConnectionCallback) {
	        _newConnectionCallback(clientSockfd, _listenAddr, peerAddr);
	    }
	}
}
#endif
