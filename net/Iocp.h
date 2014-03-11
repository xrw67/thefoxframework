#ifndef _THEFOX_NET_IOCP_H_
#define _THEFOX_NET_IOCP_H_

#include <list>
#include <Winsock2.h>
#include <base/noncopyable.h>


namespace thefox
{
namespace net
{

class Event;

class IoBuffer
{
public:
    IoBuffer(void)
    {
        _wsa.buf = _buffer;
        _wsa.len = sizeof(_buffer);
    }
    ~IoBuffer(void)
    {
    }
    int sequence() const { return _sequence; }
    void setSequence(int seq) { _sequence = seq; }
    void getBuffer(char *data, size_t &len)
    {
        data = _wsa.buf;
        len = _wsa.len;
    }
    void setBuffer(char *data, size_t len)
    {
        memcpy(_buffer, data, len);
        _wsa.len = static_cast<u_long>(len);    
    }
    void resetBuffer()
    {
        _wsa.buf = _buffer;
        _wsa.len = sizeof(_buffer); 
    }
    WSABUF &getWSABuffer() { return _wsa; }

private:
    static const int kMaxBufferSize = 8192;
	WSABUF _wsa;
	char _buffer[kMaxBufferSize];
    int _sequence;
};

class IoBufPool : noncopyable
{
public:
    static IoBufPool &getInstance()
    {
        static IoBufPool theIoBufPool;
        return theIoBufPool;
    }
    
    void push(IoBuffer *buf)
    {
        if (buf)
            _ioBuffers.push_back(buf);
    }
    IoBuffer *getBuf()
    {
        IoBuffer *buf = NULL;
        while (!_ioBuffers.empty()) {
            buf = _ioBuffers.front();
            _ioBuffers.pop_front();
            if (buf)
                break;
        }

        if (!buf)
            buf = new IoBuffer();

        return buf;
    }
private:
    ~IoBufPool(void)
    {
        while (!_ioBuffers.empty()) {
            IoBuffer *buf = _ioBuffers.front();
            _ioBuffers.pop_front();
            if (buf)
                delete buf;
        }
    }

    std::list<IoBuffer *> _ioBuffers;
};

class Iocp
{
public:
	Iocp(void);
	~Iocp(void);

	/// @brief associate a socket to this iocp;
	bool associateSocket(SOCKET s, ULONG_PTR CompletionKey);

	Event *poll();
private:
	HANDLE _hIocp;
};

} // namespace net
} // namespace thefox

#endif // _THEFOX_NET_IOCP_H_