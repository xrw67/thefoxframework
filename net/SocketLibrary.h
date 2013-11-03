#ifndef _THEFOX_SOCKETLIBRARY_H_
#define _THEFOX_SOCKETLIBRARY_H_

#include <windows.h>
#include <winsock2.h>
#include <base/noncopyable.h>
//#include <log/Logger.h>

// Need to link with Ws2_32.lib
#pragma comment(lib, "ws2_32.lib")

namespace thefox
{

class SocketLibrary : noncopyable
{
public:
	SocketLibrary()
	{
		WORD wVersionRequested;
		WSADATA wsaData;
		int err;

		wVersionRequested = MAKEWORD(2, 2);

		err = ::WSAStartup(wVersionRequested, &wsaData);
		if (err != 0) 
		{
		//	LOG_FATAL << "WSAStartup failed with error: " << err;
		}
	}
	~SocketLibrary()
	{
		::WSACleanup();
	}
};
}

#endif // _THEFOX_SOCKETLIBRARY_H_