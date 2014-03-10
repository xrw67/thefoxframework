#ifndef _THEFOX_NET_IOBUFFER_H_
#define _THEFOX_NET_IOBUFFER_H_

#include <net/win32.h>

namespace thefox
{
namespace net
{

class IoBuffer
{
public:
    IoBuffer(void);
    ~IoBuffer(void);
    int sequence() const { return _sequence; }
    void setSequence(int seq) { _sequence = seq; }
    void getBuffer(char *data, size_t &len);
    void setBuffer(char *data, size_t len);
    void resetBuffer();
    WSABUF &getWSABuffer() { return _wsa; }

private:
    static const int kMaxBufferSize = 8192;
	WSABUF _wsa;
	char _buffer[kMaxBufferSize];
    int _sequence;
};

} // namespace net
} // namespace thefox

#endif // _THEFOX_NET_IOBUFFER_H_