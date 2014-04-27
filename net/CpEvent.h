/*
* @filename CpEvent.h
* @brief 表示Socket事件
* @author macwe@qq.com
*/

#ifndef _THEFOX_NET_CPEVENT_H_
#define _THEFOX_NET_CPEVENT_H_

#include <base/MemPool.h>
#include <net/IoEvent.h>
#include <net/Callbacks.h>

namespace thefox
{

class CpEvent : public IoEvent
{
public:
    static const int kMaxBufSize = 8192;

    CpEvent(const TcpConnectionPtr &conn)
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

    void setZeroByteBuffer() 
    {
        memset(&_overlapped, 0, sizeof(OVERLAPPED));
        _wsabuf.buf = _buf;
        _wsabuf.len = 0;
    }

    TcpConnectionPtr conn() const { return _conn; }

    WSABUF wsaBuffer() const { return _wsabuf; }

private:
    char _buf[8192];
    WSABUF _wsabuf;
    TcpConnectionPtr _conn;
};

class CpEventPool : public MemPool<CpEvent>
{
public:
    static CpEventPool *instance()
    {
        static CpEventPool gCpEventPool;
        return &gCpEventPool;
    }

    CpEvent *get(TcpConnectionPtr conn)
    {
        CpEvent *ret = MemPool::get();
        new(ret) CpEvent(conn); // placement new
        return ret;
    }
    
    void put(CpEvent *ev)
    {
        ev->~CpEvent();
        MemPool::put(ev);
    }
};

} // namespace thefox

#endif // _THEFOX_NET_CPEVENT_H_