#ifndef _THEFOX_NET_IOCP_H_
#define _THEFOX_NET_IOCP_H_

#include <list>
#include <Winsock2.h>
#include <base/noncopyable.h>
#include <base/Types.h>

namespace thefox
{
namespace net
{

class IoContext
{
public:
    OVERLAPPED _overlapped;

    enum IoType{
	    kRead,
        kReadComplete,
	    kWrite,
        kWriteComplete,
        kZeroByteRead,
        kZeroByteReadComplete
    };

    IoContext(void)
    {
        _wsa.buf = _buffer;
        _wsa.len = sizeof(_buffer);
    }
    ~IoContext(void)
    {}
    uint32_t getSequence() const { return _sequence; }
    void setSequence(int seq) { _sequence = seq; }
    IoType getIoType() const { return _ioType; }
    void setIoType(IoType type) { _ioType = type; }
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
    void setZeroBuf()
    {
        _wsa.buf = _buffer;
        _wsa.len = 0;
    }
    void resetBuffer()
    {
        _wsa.buf = _buffer;
        _wsa.len = sizeof(_buffer); 
    }
    WSABUF &getWSABUF() { return _wsa; }
private:
    static const int kMaxBufferSize = 8192;
    IoType _ioType;
	WSABUF _wsa;
	char _buffer[kMaxBufferSize];
    uint32_t _sequence;
};

class IoContextPool : noncopyable
{
public:
    static IoContextPool &Instance()
    {
        static IoContextPool theIoBufPool;
        return theIoBufPool;
    }
    
    ~IoContextPool(void)
    {
        while (!_IoContexts.empty()) {
            IoContext *buf = _IoContexts.front();
            _IoContexts.pop_front();
            if (buf)
                delete buf;
        }
    }

    void put(IoContext *buf)
    {
        if (buf)
            _IoContexts.push_back(buf);
    }
    IoContext *get()
    {
        IoContext *buf = NULL;
        while (!_IoContexts.empty()) {
            buf = _IoContexts.front();
            _IoContexts.pop_front();
            if (buf)
                break;
        }

        if (NULL == buf)
            buf = new IoContext();

        return buf;
    }
private:
    std::list<IoContext *> _IoContexts;
};

class TcpConnection;

class Iocp
{
public:
	Iocp(void);
	~Iocp(void);

	/// @brief associate a socket to this iocp;
	bool registerSocket(SOCKET s, TcpConnection *conn);
    bool postCompletion(IoContext *buf, TcpConnection *conn, DWORD bytesTransferred);

	void loop();
private:
	HANDLE _hIocp;
    bool _quit;
};

} // namespace net
} // namespace thefox

#endif // _THEFOX_NET_IOCP_H_