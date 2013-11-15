#ifndef _THEFOX_NET_INETADDRESS_H
#define _THEFOX_NET_INETADDRESS_H

#include <base/copyable.h>

namespace thefox
{

class InetAddress : public copyable
{
public:
	explicit InetAddress(uint16_t port);

	InetAddress(const string& ip, uint16_t port);

	InetAddress(const struct sockaddr_in& addr)
	: _addr(addr)
	{ }

	string toIp() const;
	string toIpPort() const;
	string toHostPort() const
	{ return toIpPort(); }

	// default copy/assignment are Okay

	const struct sockaddr_in& getSockAddrInet() const { return _addr; }
	void setSockAddrInet(const struct sockaddr_in& addr) { _addr = addr; }

	uint32_t ipNetEndian() const { return _addr.sin_addr.s_addr; }
	uint16_t portNetEndian() const { return _addr.sin_port; }

private:
	struct sockaddr_in _addr;
};

}

#endif  // _THEFOX_NET_INETADDRESS_H
