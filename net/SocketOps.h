#ifndef _THEFOX_NET_SOCKETOPS_H_
#define _THEFOX_NET_SOCKETOPS_H_

#ifdef WIN32
#include <Winsock2.h>
#else
#endif
namespace thefox
{

#ifdef WIN32
class InitSocket
{
public:
    InitSocket()
    {
        WSADATA wsd;
        WSAStartup(MAKEWORD(2, 2), &wsd);
    }
    ~InitSocket()
    {
        WSACleanup();
    }
};
#define INIT_SOCKET static InitSocket;
#else
#define INIT_SOCKET
#endif



} // namespace net

#endif // _THEFOX_NET_SOCKETOPS_H_