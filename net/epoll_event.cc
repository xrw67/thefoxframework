#include <net/epoll_event.h>
#include <sys/epoll.h>
#include <log/logging.h>
#include <net/tcp_connection.h>

using namespace thefox;

EpollEvent::EpollEvent()
	: _epollfd(-1)
	, _events(32)
{
	THEFOX_TRACE_FUNCTION;
}

EpollEvent::~EpollEvent()
{
	THEFOX_TRACE_FUNCTION;

	if(-1 == ::close(_epollfd))
		THEFOX_LOG(ERROR) << "epoll close() failed";
	_epollfd = -1;
	_events.clear();
}

bool EpollEvent::init()
{
	THEFOX_TRACE_FUNCTION;

	if (-1 == _epollfd) {
		_epollfd = ::epoll_create1(EPOLL_CLOEXEC);
		
		if (-1 == _epollfd {
			THEFOX_LOG(ERROR) << "epoll_create() failed";
			return false;
		}
	}
	return true;
}

bool EpollEvent::postClose(const TcpConnectionPtr &conn)
{
	THEFOX_TRACE_FUNCTION;
	
}

bool EpollEvent::registerConnection(Tconst TcpConnectionPtr &conn)
{
	THEFOX_TRACE_FUNCTION;
	
	assert(NULL != conn);

	struct epoll_event event;
	event.events = EPOLLIN | EPOLLOUT | EPOLLET | EPOLLRDHUP;
	event.data.ptr = &*conn;

	THEFOX_LOG(DEBUG) << "epoll add connection: fd:" << conn->fd() << "ev:" << event.events;

	if (-1 == ::epoll_ctl(_epollfd, EPOLL_CTL_ADD, conn->fd(), &event)) {
		THEFOX_LOG(ERROR) << "epoll_ctl(EPOLL_CTL_ADD, " << conn->fd() << ") failed";
		return false;
	}
	return true;
}

bool EpollEvent::unregisterConnection(const TcpConnectionPtr &conn)
{
	THEFOX_TRACE_FUNCTION;

	assert(NULL != conn);

	struct epoll_event event;

	THEFOX_LOG(DEBUG) << "epoll del connection: fd:" << conn->fd();

	event.events = 0;
	event.data.ptr = NULL;

	if (-1 == ::epoll_ctl(_epollfd, EPOLL_CTL_DEL, conn->fd(), &event)) {
		THEFOX_LOG(ERROR) << "epoll_ctl(EPOLL_CTL_DEL, " << conn->fd() << ") failed";
		return false;
	}
	return true;
}

bool EpollEvent::processEvents(uint32_t timer)
{
	THEFOX_TRACE_FUNCTION;

	int numEvents = ::epoll_wait(_epollfd, &*_events.begin(),
								static_cast<int>(_events,size()),
								timer);
	int err = (-1 == numEvents) ? errno : 0;

	if (err) {
		if (EINTR != err) {
			THEFOX_LOG(ERROR) << "errno: EINTR";
		}
		THEFOX_LOG(ERROR) << "epoll_wait() failed";
		return false;
	}

	if (0 == numEvents) {
		THEFOX_LOG(TRACE) << "epoll_wait() returned 0, timeout";
		return true;
	}
	
	// resize max num of wait events;
	if (implicit_cast<size_t>(numEvents) == _events.size())
		_events.resize(events.size() * 2);
	
	for (int i = 0; i < numEvents; ++i) {
		TcpConnectionPtr conn = 
		handler(static_cast<IoEvent *>(_events[i].data.ptr), _events[i].events);
	}
}

bool EpollEvent::updateRead(IoEvent *ev)
{
	THEFOX_TRACE_FUNCTION;

}

bool EpollEvent::updateWrite(IoEvent *ev)
{
	THEFOX_TRACE_FUNCTION;

	ev->enterIo();

	TcpConnection
	Socket::write(ev->conn->fd(), )


	WSABUF wsabuf[1];
	TcpConnection *conn = ev->conn;
	Buffer *writebuf = conn->writeBuffer();

	wsabuf[0].buf = writebuf->peek();
	wsabuf[0].len = writebuf->readableBytes();

	EventOvlp *ovlp = _ovlpPool.get();
	::ZeroMemory(ovlp, sizeof(EventOvlp));
	ovlp->type = OVLP_TYPE_WRITE;
	ovlp->ev = ev;

	DWORD nBytes = 0;
    DWORD flags = 0;
    int byteSend = WSASend(conn->fd(), wsabuf, 1, 
                            &nBytes, flags, &ovlp->ovlp, NULL);
    if (SOCKET_ERROR == byteSend && WSA_IO_PENDING != WSAGetLastError()) {
        THEFOX_LOG(ERROR) << "postWrite() failed!";
		//  ¹Ø±Õconnection
		ev->leaveIo();
		_ovlpPool.put(ovlp);
		conn->connectDestroyed();
		return false;
    }
	return true;

}

void EpollEvent::handler(IoEvent *ev, uint32_t revents)
{
	THEFOX_TRACE_FUNCTION;

	if (revents & (EPOLLERR | EPOLLHUP)) {
		THEFOX_LOG(ERROR) << "error on		";
	}
	

}

void EpollEvent::handleRead(IoEvent *ev)
{

}

void EpollEvent::handleWrite(IoEvent *ev)
{

}

void EpollEvent::handleClose(IoEvent *ev)
{


}
