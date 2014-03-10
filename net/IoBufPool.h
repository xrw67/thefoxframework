#ifndef _THEFOX_NET_IOBUFPOOL_H_
#define _THEFOX_NET_IOBUFPOOL_H_

#include <list>

namespace thefox
{
namespace net
{

class IoBuffer;

class IoBufPool
{
public:
    static IoBufPool &getInstance();
    
    void push(IoBuffer *buf);
    IoBuffer *getBuf();
private:
    IoBufPool();
    IoBufPool(const IoBufPool &);
    IoBufPool &operator=(const IoBufPool &);
    ~IoBufPool(void);

    std::list<IoBuffer *> _ioBuffers;
};

} // namespace net
} // namespace thefox

#endif // _THEFOX_NET_IOBUFPOOL_H_