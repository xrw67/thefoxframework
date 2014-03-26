#ifndef _THEFOX_NET_CPEVENT_H_
#define _THEFOX_NET_CPEVENT_H_

#include <net/IoEvent.h>

namespace thefox
{

class TcpConnection;
class Tocp;

class SocketEvent : public IoEvent
{
public:
	static const int kMaxBufSize = 8192;
	SocketEvent(Iocp *iocp, TcpConnection *conn)
		: IoEvent()
		, _conn(conn)
	{
		resetBuffer();
	}
	void resetBuffer()
	{
		memset(&_overlapped, 0, sizeof(OVERLAPPED));
		_wsabuf.buf = _buf;
		_wsabuf.len = kMaxBufSize;
		memset(&_buf, 0, kMaxBufSize);
	}
	void setBuffer(const char *data, size_t len)
	{
		memset(&_overlapped, 0, sizeof(OVERLAPPED));
		_wsabuf.buf = _buf;
		_wsabuf.len = len;
		memcpy(&_buf, data, len);
	}

	TcpConnection *conn() { return _conn; }
	WSABUF wsaBuffer() { return _wsabuf; }
	char _buf[8192];
    WSABUF _wsabuf;
	TcpConnection *_conn;
	Iocp *_iocp;
};

} // namespace thefox

#endif // _THEFOX_NET_CPEVENT_H_