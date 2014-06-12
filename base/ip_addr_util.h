#ifndef _THEFOX_BASE_IP_ADDR_UTIL_H_
#define _THEFOX_BASE_IP_ADDR_UTIL_H_

#include <base/common.h>

namespace thefox
{

class IpAddrUtil
{
public:
	static string toString(uint32_t ip)
	{
		char buf[32] = {0};
		uint8_t* ptr = (uint8_t*)&ip;
		snprintf(buf, sizeof(buf) - 1, "%u.%u.%u.%u", ptr[3], ptr[2], ptr[1], ptr[0]);
		buf[sizeof(buf) - 1] = '\0';
		return buf;
	}

	static uint32_t stoi(const char* ipStr)
	{
		int32_t ip, d0, d1, d2, d3;
		sscanf(ipStr, "%d.%d.%d.%d", &d0, &d1, &d2, &d3);
		ip = (((((d0<<8)+d1)<<8)+d2)<<8)+d3;
		return ip;
	}
	
	static uint32_t stoi(const string &ipStr)
	{
		return stoi(ipStr.c_str());
	}
};

} // namespace thefox

#endif // _THEFOX_BASE_IPADDRUTIL_H_