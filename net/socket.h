#ifndef _THEFOX_NET_SOCKET_H_
#define _THEFOX_NET_SOCKET_H_

#ifdef WIN32
#include <Winsock2.h>
#define socklen_t int

/// @breaf socket库初始化
#define THEFOX_SOCKET_STARTUP               \
	WSADATA wsd;                            \
	::WSAStartup(MAKEWORD(2, 2), &wsd);

/// @breaf socket库注销
#define THEFOX_SOCKET_CLEANUP               \
	::WSACleanup();

#else
	#include <sys/types.h>
	#include <sys/socket.h>
    typedef int SOCKET
	#define INVALID_SOCKET -1
	#define SOCKET_ERROR -1
	#define THEFOX_SOCKET_STARTUP
	#define THEFOX_SOCKET_CLEANUP
#endif

#include <base/common.h>

namespace thefox
{

class InetAddress;

class Socket
{
public:
	explicit Socket(SOCKET sockfd);
	~Socket();

	static InetAddress getLocalAddr(SOCKET sockfd);
	static SOCKET create();
	static bool connect(SOCKET sockfd, const InetAddress &serverAddr);
	static bool close(SOCKET sockfd);
	
	bool bind(const InetAddress &listenAddr);
	bool listen();
	SOCKET accept(InetAddress *peerAddr);
	
	bool shutdownWrite();
	// 取消nagle算法
	void setTcpNoDelay(bool on);
	void setKeepAlive(bool on);

	SOCKET fd() const { return _sockfd; }

private:
	SOCKET _sockfd;
};

} // namespace thefox

#endif // _THEFOX_NET_SOCKET_H_