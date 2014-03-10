#include <net/Socket.h>
#include <net/InetAddress.h>

using namespace thefox;
using namespace thefox::net;

SOCKET Socket::Create()
{
    return ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

Socket::Socket(SOCKET s)
	:_s(s)
{
}

Socket::~Socket(void)
{
    ::closesocket(_s);
}

void Socket::listen(const InetAddress &listenAddr)
{
    ::bind(_s,
         (struct sockaddr *)&listenAddr.getSockAddrInet(),
         sizeof(struct sockaddr_in));
    ::listen(_s, 5);
}