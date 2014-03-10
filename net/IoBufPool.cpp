#include <net/IoBufPool.h>
#include <net/IoBuffer.h>

using namespace thefox;
using namespace thefox::net;

IoBufPool &IoBufPool::getInstance()
{
    static IoBufPool sIoBufPool;
    return sIoBufPool;
}

IoBufPool::~IoBufPool()
{
    while (!_ioBuffers.empty()) {
        IoBuffer *buf = _ioBuffers.front();
        _ioBuffers.pop_front();
        if (buf)
            delete buf;
    }
}

void IoBufPool::push(IoBuffer *buf)
{
    if (buf)
        _ioBuffers.push_back(buf);
}

IoBuffer *IoBufPool::getBuf()
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