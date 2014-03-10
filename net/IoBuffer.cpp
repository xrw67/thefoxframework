#include <net/IoBuffer.h>

using namespace thefox;
using namespace thefox::net;

IoBuffer::IoBuffer(void)
{
    _wsa.buf = _buffer;
    _wsa.len = sizeof(_buffer);
}

IoBuffer::~IoBuffer(void)
{
}

void IoBuffer::getBuffer(char *data, size_t &len)
{
    data = _wsa.buf;
    len = _wsa.len;
}
void IoBuffer::setBuffer(char *data, size_t len)
{
    memcpy(_buffer, data, len);
    _wsa.len = static_cast<u_long>(len);
}

void IoBuffer::resetBuffer()
{
    _wsa.buf = _buffer;
    _wsa.len = sizeof(_buffer);
}
