/*
* @filename SocketEvent.h
* @brief ��ʾSocket�¼�
* @author macwe@qq.com
*/

#ifndef _THEFOX_NET_SOCKETEVENT_H_
#define _THEFOX_NET_SOCKETEVENT_H_

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
		, _iocp(iocp)
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

	void setZeroByteBuffer() 
	{
		memset(&_overlapped, 0, sizeof(OVERLAPPED));
		_wsabuf.buf = _buf;
		_wsabuf.len = 0;
	}

	Iocp *iocp() const { return _iocp; }

	TcpConnection *conn() const { return _conn; }

	WSABUF wsaBuffer() const { return _wsabuf; }

private:
	char _buf[8192];
    WSABUF _wsabuf;
	TcpConnection *_conn;
	Iocp *_iocp;
};

} // namespace thefox

#endif // _THEFOX_NET_SOCKETEVENT_H_