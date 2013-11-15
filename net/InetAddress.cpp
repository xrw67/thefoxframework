#include <net/InetAddress.h>

using namespace thefox;

InetAddress::InetAddress(uint16_t port)
{
	memset(&_addr, 0, sizeof(_addr);
	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	_addr.sin_port = htons(port);
}

InetAddress::InetAddress(const string &ip, uint16_t port)
{
	memset(&_addr, 0, sizeof(_addr));
	_addr->sin_family = AF_INET;
	_addr->sin_addr.s_addr = inet_addr(ip.c_str());
	_addr->sin_port = htons(port);
}

string InetAddress::toIpPort() const
{
	char buf[32];
	snprintf(buf, sizeof(buf), "%s:%u", toIp().c_str(), ntohs(_addr.sin_port));
	return buf;
}

string InetAddress::toIp() const
{
	char *host = inet_ntoa(_addr.sin_addr);
	if (NULL != host)
		return host;
	else
		return "INVALID";
}

