#include <net/epoll_event.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <log/logging.h>
#include <net/buffer.h>
#include <net/tcp_connection.h>

using namespace thefox;
using namespace thefox::net;

EpollEvent::EpollEvent()
	: _epollfd(-1)
	, _events(32)
{}

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
		
		if (-1 == _epollfd) {
			THEFOX_LOG(ERROR) << "epoll_create() failed";
			return false;
		}
	}
	return true;
}

bool EpollEvent::addEvent(Event *ev)
{
	THEFOX_TRACE_FUNCTION;
	
	assert(NULL != ev);

	ev->handler = std::bind(&EpollEvent::handler, this, _1, _2);

	struct epoll_event event;
	event.events = EPOLLIN | EPOLLOUT | EPOLLET | EPOLLRDHUP;
	event.data.ptr = ev;

	THEFOX_LOG(DEBUG) << "epoll add connection: fd:" << ev->fd << "ev:" << event.events;

	if (-1 == ::epoll_ctl(_epollfd, EPOLL_CTL_ADD, ev->fd, &event)) {
		THEFOX_LOG(ERROR) << "epoll_ctl(EPOLL_CTL_ADD, " << ev->fd << ") failed";
		return false;
	}
	return true;
}

bool EpollEvent::delEvent(Event *ev)
{
	THEFOX_TRACE_FUNCTION;

	assert(NULL != ev);

	struct epoll_event event;

	THEFOX_LOG(DEBUG) << "epoll del connection: fd:" << ev->fd;

	event.events = 0;
	event.data.ptr = NULL;

	if (-1 == ::epoll_ctl(_epollfd, EPOLL_CTL_DEL, ev->fd, &event)) {
		THEFOX_LOG(ERROR) << "epoll_ctl(EPOLL_CTL_DEL, " << ev->fd << ") failed";
		return false;
	}
	return true;
}

bool EpollEvent::processEvents(uint32_t timer)
{
	THEFOX_TRACE_FUNCTION;

	int numEvents = ::epoll_wait(_epollfd, &*_events.begin(),
								static_cast<int>(_events.size()),
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
	if (static_cast<size_t>(numEvents) == _events.size()) {
		_events.resize(_events.size() * 2);
	}
	
	for (int i = 0; i < numEvents; ++i) {
		Event *ev = reinterpret_cast<Event *>(_events[i].data.ptr);
		ev->handler(ev, (void *)_events[i].events);
	}
}

bool EpollEvent::updateWrite(Event *ev)
{
	THEFOX_TRACE_FUNCTION;

	int ret;
	TcpConnection *conn = ev->conn;
	Buffer *buffer = conn->writeBuffer();

	while (buffer->readableBytes() > 0) {
		ret = write(ev->fd, buffer->peek(), buffer->readableBytes());
		
		if (-1 == ret && EAGAIN != errno) {
			THEFOX_LOG(ERROR) << "write error";
			return false;
		}

		buffer->retrieve(ret);
	}

	if (conn->_writeCompleteCallback) {
		conn->_writeCompleteCallback(conn);
	}

	return true;
}

void EpollEvent::handler(Event *ev, void *arg)
{
	THEFOX_TRACE_FUNCTION;

	uint32_t revents = (uint32_t)arg;

	if (revents & (EPOLLERR | EPOLLHUP)) {
		THEFOX_LOG(ERROR) << "error on		";
		onClose(ev, revents);
	}
	if (revents & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
		onRead(ev);
	}
	if (revents & EPOLLOUT) {
		onWrite(ev);
	}
}

bool EpollEvent::onRead(Event *ev)
{
	THEFOX_TRACE_FUNCTION;

	int ret;
	TcpConnection *conn = ev->conn;
	Buffer *buffer = conn->readBuffer();

	buffer->ensureWritableBytes(kDefaultBufferSize);

	while ((ret = ::read(ev->fd, buffer->beginWrite(), kDefaultBufferSize)) > 0) {
		conn->addReadBytes(ret);
	}

	if (-1 == ret && EAGAIN != errno) {
		THEFOX_LOG(ERROR) << "read error";
		return false;
	} // else 读完成
	return true;
}

bool EpollEvent::onWrite(Event *ev)
{
	THEFOX_TRACE_FUNCTION;

	int ret;
	TcpConnection *conn = ev->conn;
	Buffer *buffer = conn->writeBuffer();

	while (buffer->readableBytes() > 0) {
		ret = ::write(conn->fd(), buffer->peek(), buffer->readableBytes());
		if (-1 == ret && EAGAIN != errno) {
			THEFOX_LOG(ERROR) << "write error";
			return false;
		}

		buffer->retrieve(ret);
	}
	return true;
}

bool EpollEvent::onClose(Event *ev, uint32_t revents)
{
	TcpConnection *conn = ev->conn;

	if (NULL == conn) {
		return false;
	}

	conn->connectDestroyed();
	return true;
}
