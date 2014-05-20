#ifndef _THEFOX_NET_SOCKET_H_
#define _THEFOX_NET_SOCKET_H_

#ifdef WIN32
#include <Winsock2.h>
#define socklen_t int

class SocketLibrary
{
public:
	/// @breaf socket¿â³õÊ¼»¯
    static void startup()
	{
        WSADATA wsd;
        ::WSAStartup(MAKEWORD(2, 2), &wsd);
    }

	/// @breaf socket¿â×¢Ïú
    static void cleanup() { ::WSACleanup(); }
};

#else
#include <sys/types.h>
#include <sys/socket.h>
#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#endif

#include <base/types.h>

namespace thefox
{

class InetAddress;

class Socket
{
public:
	Socket();
	~Socket();

	bool create();
	bool bind(const InetAddress &listenAddr);
	bool listen();
	SOCKET accept(InetAddress *peerAddr);
	bool connect(const InetAddress &serverAddr);
	bool close();
	bool shutdownWrite();
	void setTcpNoDelay(bool on);
	void setKeepAlive(bool on);

	SOCKET fd() const { return _sockfd; }

private:
	SOCKET _sockfd;
};

} // namespace thefox

#endif // _THEFOX_NET_SOCKET_H_