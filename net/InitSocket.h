#ifndef _THEFOX_NET_INITSOCKET_H_
#define _THEFOX_NET_INITSOCKET_H_

#include <net/inc.h>

namespace thefox
{

class InitSocket
{
public:
	InitSocket(LPWSADATA lpWSAData = nullptr, BYTE minorVersion = 2, BYTE majorVersion = 2)
	{
		LPWSADATA temp = lpWSAData;
		if(!temp)
			temp = (LPWSADATA)_alloca(sizeof(WSADATA));

		_result = ::WSAStartup(MAKEWORD(minorVersion, majorVersion), temp);
	}

	~InitSocket()
	{
		if (IsValid())
			::WSACleanup();
	}

	int	GetResult() {return _result;}
	BOOL IsValid() {return _result == 0;}

private:
	int _result;
};

}

#endif // _THEFOX_NET_INITSOCKET_H_