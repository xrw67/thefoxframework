/*
 * @filename SyslogD.h
 * @beirf 这是Syslog服务器的一个实现类
 *
 */

#ifndef _THEFOX_NET_SYSLOGD_H_
#define _THEFOX_NET_SYSLOGD_H_

#include <base/noncopyable>
#include <net/IpAddress.h>

class SyslogInfo;

namespace thefox
{

namespace net
{
    
class SyslogD :noncopyable
{
public:
    SyslogD(const IpAddress *listenAddr)
        : _listenAddr(listenAddr);
    {}
    ~SyslogD()
    {}
    bool listen()
    {}
    void messageCallback(const IpAddress &peerAddr, const SyslogInfo &syslog);
    
private:
    IpAddress _listenAddr;
}
    
}; // net
    
}; // namespace thefox

#endif // _THEFOX_NET_SYSLOGD_H_