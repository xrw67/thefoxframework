#ifndef _THEFOX_IPADDRESS_H_
#define _THEFOX_IPADDRESS_H_

#include <base/Types.h>
#include <base/copyable.h>
#include <base/StringUtil.h>
#include <winsock2.h>

namespace thefox
{

class InetAddress : public copyable
{
public:
	explicit InetAddress(uint16_t port)
	{
		memset(&_addr, 0, sizeof(_addr));
		_addr.sin_family = AF_INET;
		_addr.sin_addr.s_addr = INADDR_ANY;
		_addr.sin_port = htons(port);
	}
	InetAddress(const String& ip, uint16_t port)
	{
		memset(&_addr, 0, sizeof(_addr));
		_addr.sin_family = AF_INET;
		_addr.sin_addr.s_addr = inet_addr(ip.c_str());
		_addr.sin_port = htons(port);
	}
	InetAddress(const struct sockaddr_in &addr)
	: _addr(addr)
	{
	}

	String toIp() const
	{
		return String(inet_ntoa(_addr.sin_addr));
	}
	String toIpPort() const
	{
		return String(toIp() + ":" + toString(ntohs(_addr.sin_port)));
	}

	const struct sockaddr_in& getSockAddrInet() const { return _addr; }
	void setSockAddrInet(const struct sockaddr_in& addr) { _addr = addr; }

	uint32_t ipNetEndian() const { return _addr.sin_addr.s_addr; }
	uint16_t portNetEndian() const { return _addr.sin_port; }
private:
	struct sockaddr_in _addr;
};

}

#endif // _THEFOX_IPASSRESS_H_