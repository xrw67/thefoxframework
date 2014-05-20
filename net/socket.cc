#include <net/socket.h>
#include <log/logging.h>
#include <net/inet_address.h>

using namespace thefox;

Socket::Socket()
	: _sockfd(INVALID_SOCKET)
{
}

Socket::~Socket()
{
	this->close();
}

bool Socket::create()
{
#ifdef WIN32
	_sockfd = ::WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == _sockfd) {
        int errCode = ::WSAGetLastError();
        THEFOX_LOG(ERROR) << "WSASocket failed, errcode:"<< errCode;
        return false;
    }
#else
	_sockfd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (SOCKET_ERROR == _sockfd) {
		THEFOX_LOG(ERROR) << "create socket failed";
		return false;
	}
#endif
	return true;
}

bool Socket::bind(const InetAddress &listenAddr)
{
	int ret = ::bind(_sockfd, (struct sockaddr *)&listenAddr.getSockAddrInet(), sizeof(struct sockaddr_in));
    
	if (SOCKET_ERROR == ret) {
#ifdef WIN32
        int errCode = ::WSAGetLastError();
        THEFOX_LOG(ERROR) << "socket bind failed, errcode:"<< errCode;
#else
		THEFOX_LOG(ERROR) << "socket bind failed";
#endif
        this->close();
        return false;
    }
	return true;
}

bool Socket::listen()
{
	int ret = ::listen(_sockfd, SOMAXCONN);
    if (SOCKET_ERROR == ret) {
#ifdef WIN32
        int errCode = ::WSAGetLastError();
        THEFOX_LOG(ERROR) << "socket listen failed, errcode:"<< errCode;
#else
		THEFOX_LOG(ERROR) << "socket listen failed";
#endif
        this->close();
        return false;
    }
	return true;
}

SOCKET Socket::accept(InetAddress *peerAddr)
{
	SOCKET clientSockfd = INVALID_SOCKET;
	struct sockaddr_in addr;
    int len = sizeof(addr);
#ifdef WIN32
    clientSockfd = ::WSAAccept(_sockfd, (sockaddr *)&addr, &len, 0, 0);
#else
	clientSockfd = ::accept(_sockfd, (sockaddr *)&addr, &len);
#endif
	if (INVALID_SOCKET == clientSockfd) {
		THEFOX_LOG(ERROR) << "accept socket failed";
	} else {
		peerAddr->setSockAddrInet(addr);
	}
	return clientSockfd;
}

bool Socket::connect(const InetAddress &serverAddr)
{
	int ret = ::connect(_sockfd, 
                      (struct sockaddr *)&serverAddr.getSockAddrInet(), 
                      sizeof(struct sockaddr_in));

    if (INVALID_SOCKET == ret  
#ifdef WIN32
		&& WSAEWOULDBLOCK != ::WSAGetLastError()
#endif
		) {
		THEFOX_LOG(ERROR) << "server connection failed, addr="<< serverAddr.toIpPort();
        this->close();
        return false;
	}
	return true;
}

bool Socket::close()
{
	if (INVALID_SOCKET != _sockfd) {
#ifdef WIN32
		::shutdown(_sockfd, SD_BOTH);
		if (SOCKET_ERROR == ::closesocket(_sockfd)) {
#else
		::shutdown(_sockfd, SHUT_RDWR);
		if (-1 == ::close(_sockfd)) {
#endif
			return false;
		}
		_sockfd = INVALID_SOCKET;
	}
	return true;
}

bool Socket::shutdownWrite()
{
#ifdef WIN32
	if (SOCKET_ERROR == ::shutdown(_sockfd, SD_SEND)) {
#else
	if (-1 == ::shutdown(_sockfd, SHUT_WR)) {
#endif
		return false;
	}
	return true;
}

void Socket::setTcpNoDelay(bool on)
{
	char optval = on ? 1 : 0;
	::setsockopt(_sockfd, SOL_SOCKET, TCP_NODELAY,
               &optval, static_cast<socklen_t>(sizeof optval));
}

void Socket::setKeepAlive(bool on)
{
	char optval = on ? 1 : 0;
	::setsockopt(_sockfd, SOL_SOCKET, SO_KEEPALIVE,
               &optval, static_cast<socklen_t>(sizeof optval));
}