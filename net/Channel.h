#ifndef _THEFOX_NET_CHANNEL_H
#define _THEFOX_NET_CHANNEL_H

#include <functionl>
#include <base/noncopyable.hpp>
#include <memory>
#include <base/Timestamp.h>

namespace thefox
{
class EventLoop;

///
/// A selectable I/O channel.
///
/// This class doesn't own the file descriptor.
/// The file descriptor could be a socket,
/// an eventfd, a timerfd, or a signalfd
class Channel : noncopyable
{
public:
	typedef boost::function<void()> EventCallback;
	typedef boost::function<void(Timestamp)> ReadEventCallback;

	Channel(EventLoop* loop, int fd);
	~Channel();

	void handleEvent(Timestamp receiveTime);
	void setReadCallback(const ReadEventCallback& cb) { _readCallback = cb; }
	void setWriteCallback(const EventCallback& cb) { _writeCallback = cb; }
	void setCloseCallback(const EventCallback& cb) { _closeCallback = cb; }
	void setErrorCallback(const EventCallback& cb) { _errorCallback = cb; }

	/// Tie this channel to the owner object managed by shared_ptr,
	/// prevent the owner object being destroyed in handleEvent.
	void tie(const boost::shared_ptr<void>&);

	int fd() const { return fd_; }
	int events() const { return events_; }
	void set_revents(int revt) { revents_ = revt; } // used by pollers
	// int revents() const { return revents_; }
	bool isNoneEvent() const { return events_ == kNoneEvent; }

	void enableReading() { _events |= kReadEvent; update(); }
	// void disableReading() { _events &= ~kReadEvent; update(); }
	void enableWriting() { _events |= kWriteEvent; update(); }
	void disableWriting() { _events &= ~kWriteEvent; update(); }
	void disableAll() { _events = kNoneEvent; update(); }
	bool isWriting() const { return _events & kWriteEvent; }

	// for Poller
	int index() { return index_; }
	void set_index(int idx) { index_ = idx; }

	// for debug
	String reventsToString() const;

	void doNotLogHup() { _logHup = false; }

	EventLoop* ownerLoop() { return _loop; }
	void remove();

private:
	void update();
	void handleEventWithGuard(Timestamp receiveTime);

	static const int kNoneEvent;
	static const int kReadEvent;
	static const int kWriteEvent;

	EventLoop *_loop;
	const int _fd;
	int _events;
	int _revents;
	int _index; // used by Poller.
	bool _logHup;

	std::weak_ptr<void> _tie;
	bool _tied;
	bool _eventHandling;
	ReadEventCallback _readCallback;
	EventCallback _writeCallback;
	EventCallback _closeCallback;
	EventCallback _errorCallback;
};

}
#endif  // _THEFOX_NET_CHANNEL_H
