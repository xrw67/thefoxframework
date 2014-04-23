#ifndef _THEFOX_NET_SOCKETOPS_H_
#define _THEFOX_NET_SOCKETOPS_H_

#ifdef WIN32
#include <Winsock2.h>
#else
#endif

namespace thefox
{

class SocketLibrary
{
public:
	/// @breaf socket¿â³õÊ¼»¯
    static void startup()
    {
#ifdef WIN32
        WSADATA wsd;
        WSAStartup(MAKEWORD(2, 2), &wsd);
#endif
    }

	/// @breaf socket¿â×¢Ïú
    static void shutdown()
    { 
#ifdef WIN32
		WSACleanup(); 
#endif
	}
};

} // namespace thefox

#endif // _THEFOX_NET_SOCKETOPS_H_